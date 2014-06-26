#include "html2mark.h"
#include "xmlreader.h"
#include <chthon2/log.h>
#include <chthon2/util.h>
#include <sstream>
#include <vector>
#include <cctype>

namespace Html2Mark {

std::string collapse(const std::string & data,
		bool remove_heading, bool remove_trailing)
{
	std::string result;
	bool is_space_group = false;
	for(char c : data) {
		if(isspace(c)) {
			if(!is_space_group) {
				result += ' ';
				is_space_group = true;
			}
		} else {
			result += c;
			is_space_group = false;
		}
	}
	if(remove_heading && !result.empty() && result[0] == ' ') {
		result.erase(0, 1);
	}
	if(remove_trailing && !result.empty() && result[result.size() - 1] == ' ') {
		result.erase(result.size() - 1, 1);
	}
	return result;
}

struct TaggedContent {
	std::string tag, content;
	typedef std::map<std::string, std::string> Attrs;
	Attrs attrs;

	TaggedContent(const std::string & given_tag = std::string(),
			const std::string & given_content = std::string(),
			const Attrs & given_attrs = Attrs()
			)
		: tag(given_tag), content(given_content), attrs(given_attrs)
	{}
	std::string process_tag() const;
};

std::string TaggedContent::process_tag() const
{
	if(tag == "p") {
		return "\n" + content + "\n";
	}
	if(tag == "") {
		return content;
	}
	return Chthon::format("<{0}>{1}</{0}>", tag, content);
}

struct List {
	bool numbered;
	std::vector<std::string> items;
	List(bool numbered_list) : numbered(numbered_list) {}
};

struct Html2MarkProcessor {
	Html2MarkProcessor(const std::string & html, int html_options,
			int html_min_reference_links_length);
	void process();
	const std::string & get_result() const { return result; }
private:
	std::istringstream stream;
	int options;
	int min_reference_links_length;
	std::string result;
	std::vector<TaggedContent> parts;
	std::vector<std::pair<unsigned, std::string>> references;
	std::vector<List> lists;

	std::string process_tag(const TaggedContent & value);
	void collapse_tag(const std::string & tag = std::string());
	void add_content(const std::string & content);
};

Html2MarkProcessor::Html2MarkProcessor(const std::string & html, int html_options,
		int html_min_reference_links_length)
	: stream(html), options(html_options),
	min_reference_links_length(html_min_reference_links_length)
{}

std::string Html2MarkProcessor::process_tag(const TaggedContent & value)
{
	if(value.tag.empty()) {
		return value.content;
	} else if(value.tag == "p") {
		return "\n" + collapse(value.content, true, true) + "\n";
	} else if(value.tag == "em") {
		return value.content.empty() ? "" : "_" + value.content + "_";
	} else if(value.tag == "i") {
		return value.content.empty() ? "" : "_" + value.content + "_";
	} else if(value.tag == "b") {
		return value.content.empty() ? "" : "**" + value.content + "**";
	} else if(value.tag == "strong") {
		return value.content.empty() ? "" : "**" + value.content + "**";
	} else if(value.tag == "code") {
		return value.content.empty() ? "" : "`" + value.content + "`";
	} else if(value.tag == "ol" || value.tag == "ul") {
		if(lists.empty()) {
			return "\n" + collapse(value.content, true, true) + "\n";
		}
		std::string content;
		if(!value.content.empty()) {
			content = "\n" + collapse(value.content, true, true) + "\n";
		}
		content += '\n';
		int index = 1;
		for(const std::string & item : lists.back().items) {
			std::vector<std::string> lines;
			Chthon::split(item, lines);
			bool is_first_line = true;
			for(const std::string & line : lines) {
				if(is_first_line) {
					std::string number;
					if(lists.back().numbered) {
						number = std::to_string(index) + ". ";
					} else {
						number = "* ";
					}
					content += number + line + "\n";
					is_first_line = false;
				} else {
					content += "  " + line + "\n";
				}
			}
			++index;
		}
		lists.pop_back();
		return content;
	} else if(value.tag == "li") {
		if(lists.empty()) {
			return "\n" + collapse(value.content, true, true) + "\n";
		}
		lists.back().items.push_back(value.content);
		return "";
	} else if(Chthon::starts_with(value.tag, "h")) {
		if(value.content.empty()) {
			return "";
		}
		size_t level = strtoul(value.tag.substr(1).c_str(), nullptr, 10);
		if(level < 1 || 6 < level) {
			return Chthon::format("<{0}>{1}</{0}>", value.tag, value.content);
		}
		std::string content = collapse(value.content, true, true);
		if(level <= 2 && options & UNDERSCORED_HEADINGS) {
			char underscore = level == 1 ? '=' : '-';
			return "\n" + content + "\n" +
				std::string(content.size(), underscore) + "\n";
		}
		return "\n" + std::string(level, '#') +  " " + content + "\n";
	} else if(value.tag == "a") {
		if(value.attrs.count("href") == 0) {
			return value.content;
		}
		std::string src = value.attrs.at("href");
		if(value.attrs.count("title")) {
			src += " \"" + value.attrs.at("title") + '"';
		}
		bool is_too_long = value.attrs.at("href").size() > min_reference_links_length;
		std::string content = collapse(value.content);
		if(options & MAKE_REFERENCE_LINKS && is_too_long) {
			unsigned ref_number = references.size() + 1;
			references.emplace_back(ref_number, src);
			return Chthon::format("[{0}][{1}]", content, ref_number);
		} else {
			return Chthon::format("[{0}]({1})", content, src);
		}
	} else if(value.tag == "pre") {
		std::vector<std::string> lines;
		Chthon::split(value.content, lines);
		std::string content;
		for(const std::string & line : lines) {
			content += "\n\t" + line;
		}
		return content + "\n";
	} else if(value.tag == "blockquote") {
		std::vector<std::string> lines;
		Chthon::split(value.content, lines);
		std::string content;
		for(const std::string & line : lines) {
			content += "\n> " + line;
		}
		return content + "\n";
	}
	return Chthon::format("<{0}>{1}</{0}>", value.tag, value.content);
}

void Html2MarkProcessor::add_content(const std::string & content)
{
	if(parts.empty()) {
		result += content;
	} else {
		parts.back().content += content;
	}
}

void Html2MarkProcessor::collapse_tag(const std::string & tag)
{
	while(!parts.empty()) {
		TaggedContent value = parts.back();
		parts.pop_back();
		add_content(process_tag(value));
		if(value.tag == tag) {
			break;
		}
	}
}

void Html2MarkProcessor::process()
{
	XMLReader reader(stream);

	std::string tag = reader.to_next_tag();
	std::string content = collapse(reader.get_current_content());
	std::map<std::string, std::string> attrs = reader.get_attributes();
	result += content;
	while(!tag.empty()) {
		reader.to_next_tag();
		content = reader.get_current_content();

		/*/
		TRACE("----");
		TRACE(tag);
		TRACE(content);
		//*/

		if(Chthon::starts_with(tag, "/")) {
			std::string open_tag = tag.substr(1);
			bool found = parts.rend() != std::find_if(
					parts.rend(), parts.rbegin(),
					[&open_tag](const TaggedContent & value) {
						return value.tag == open_tag;
					}
					);
			if(found) {
				collapse_tag(open_tag);
			}
			add_content(content);
		} else if(tag == "code") {
			if(!parts.empty() && parts.back().tag == "pre" && parts.back().content.empty()) {
				parts.back().content = content;
			} else {
				parts.emplace_back(tag, content, attrs);
			}
		} else if(tag == "ol" || tag == "ul") {
			lists.push_back(List(tag == "ol"));
			parts.emplace_back(tag, content, attrs);
		} else if(tag == "li") {
			bool list_found = false, li_found = false;
			for(const TaggedContent & part : parts) {
				if(part.tag == "ol" || part.tag == "ul") {
					list_found = true;
					li_found = false;
				} else if(part.tag == "li") {
					if(list_found) {
						li_found = true;
					}
				}
			}
			if(li_found) {
				collapse_tag("li");
			}
			parts.emplace_back(tag, collapse(content, true, true), attrs);
		} else if(tag == "p") {
			bool found = false;
			for(const TaggedContent & value : parts) {
				if(value.tag == "p") {
					found = true;
					break;
				}
			}
			if(found) {
				collapse_tag("p");
			}
			parts.emplace_back(tag, content, attrs);
		} else if(Chthon::starts_with(tag, "hr")) {
			add_content("\n* * *\n");
			add_content(content);
		} else if(Chthon::starts_with(tag, "br")) {
			add_content("\n");
			add_content(content);
		} else if(tag == "img") {
			std::string src = attrs["src"];
			if(attrs.count("title")) {
				src += " \"" + attrs["title"] + '"';
			}
			bool is_too_long = attrs["src"].size() > min_reference_links_length;
			if(options & MAKE_REFERENCE_LINKS && is_too_long) {
				unsigned ref_number = references.size() + 1;
				references.emplace_back(ref_number, src);
				add_content(Chthon::format("![{0}][{1}]", attrs["alt"], ref_number));
			} else {
				add_content(Chthon::format("![{0}]({1})", attrs["alt"], src));
			}
			add_content(content);
		} else {
			parts.emplace_back(tag, content, attrs);
		}

		tag = reader.get_current_tag();
		attrs = reader.get_attributes();
	}
	collapse_tag();
	if(!references.empty()) {
		result += "\n\n";
		for(auto ref : references) {
			result += Chthon::format("[{0}]: {1}\n", ref.first, ref.second);
		}
	}
}

std::string html2mark(const std::string & html, int options,
		int min_reference_links_length)
{
	Html2MarkProcessor processor(html, options, min_reference_links_length);
	processor.process();
	return processor.get_result();
}

}
