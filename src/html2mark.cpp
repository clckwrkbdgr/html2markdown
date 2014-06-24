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

struct Html2MarkProcessor {
	Html2MarkProcessor(const std::string & in_html);
	void process_html();
	void process_parts();
	const std::string & get_result() const;
private:
	typedef std::pair<std::string, std::string> ContentTag;
	const std::string & html;
	std::string result;
	std::vector<ContentTag> parts;
};

Html2MarkProcessor::Html2MarkProcessor(const std::string & in_html)
	: html(in_html)
{
}

const std::string & Html2MarkProcessor::get_result() const
{
	return result;
}

void Html2MarkProcessor::process_html()
{
	std::istringstream stream(html);
	XMLReader reader(stream);

	std::string tag = reader.to_next_tag();
	std::string content = collapse(reader.get_current_content(), true, true);
	parts << ContentTag(content, tag);
	while(!tag.empty()) {
		tag = reader.to_next_tag();
		content = collapse(reader.get_current_content(), true, true);
		if(!tag.empty() || !content.empty()) {
			parts << ContentTag(content, tag);
		}
	}
}

void Html2MarkProcessor::process_parts()
{
	//TRACE("----");
	bool in_paragraph = false;
	for(const ContentTag & content_tag : parts) {
		const std::string & content = content_tag.first;
		const std::string & tag = content_tag.second;
		//TRACE(content_tag.first);
		//TRACE(content_tag.second);

		result += content;
		if(tag == "p") {
			if(in_paragraph) {
				result += '\n';
			}
			in_paragraph = true;
			result += '\n';
		}
		if(tag == "/p") {
			in_paragraph = false;
			result += '\n';
		}
	}
	if(in_paragraph) {
		result += '\n';
	}
}


std::string html2mark(const std::string & html, int /*options*/,
		int /*min_reference_links_length*/)
{
	Html2MarkProcessor process(html);
	process.process_html();
	process.process_parts();
	return process.get_result();
}

}
