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
	TaggedContent(const std::string & given_tag = std::string(),
			const std::string & given_content = std::string())
		: tag(given_tag), content(given_content)
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

static std::string process_tag(const TaggedContent & value)
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
	}
	return Chthon::format("<{0}>{1}</{0}>", value.tag, value.content);
}

static std::string collapse_tag(std::vector<TaggedContent> & parts, const std::string & tag = std::string())
{
	std::string result;
	while(!parts.empty()) {
		TaggedContent value = parts.back();
		parts.pop_back();
		if(parts.empty()) {
			result += process_tag(value);
		} else {
			parts.back().content += process_tag(value);
		}
		if(value.tag == tag) {
			break;
		}
	}
	return result;
}

std::string html2mark(const std::string & html, int /*options*/,
		int /*min_reference_links_length*/)
{
	std::istringstream stream(html);
	XMLReader reader(stream);

	std::string result;
	std::vector<TaggedContent> parts;
	std::string tag = reader.to_next_tag();
	std::string content = collapse(reader.get_current_content(), true, true);
	result += content;
	while(!tag.empty()) {
		reader.to_next_tag();
		content = reader.get_current_content();

		if(Chthon::starts_with(tag, "/")) {
			std::string open_tag = tag.substr(1);
			bool found = parts.rend() != std::find_if(
					parts.rend(), parts.rbegin(),
					[&open_tag](const TaggedContent & value) {
						return value.tag == open_tag;
					}
					);
			if(found) {
				result += collapse_tag(parts, open_tag);
			}
		} else {
			if(tag == "p") {
				result += collapse_tag(parts);
			}
			parts.emplace_back(tag, content);
		}

		tag = reader.get_current_tag();
	}
	result += collapse_tag(parts);
	return result;
}

}
