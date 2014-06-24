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
	enum { NONE, P, EM, I };

	const std::string & html;
	std::string result;
	std::vector<ContentTag> parts;
	int mode;
	bool was_paragraph;
	std::string prepared_content;

	void process_tag(const std::string & tag, bool content_is_empty = false);
};

Html2MarkProcessor::Html2MarkProcessor(const std::string & in_html)
	: html(in_html), mode(NONE), was_paragraph(false)
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

struct Token {
	enum { UNKNOWN, TRUE, FALSE };

	std::string appendix;
	int new_mode;
	std::string appendix_to_remove;
	int was_paragraph;
	Token(const std::string & token_appendix, int token_new_mode,
			const std::string & token_appendix_to_remove = std::string(),
			int token_was_paragraph = UNKNOWN)
		: appendix(token_appendix), new_mode(token_new_mode),
		appendix_to_remove(token_appendix_to_remove),
		was_paragraph(token_was_paragraph)
	{}
};

void Html2MarkProcessor::process_tag(const std::string & tag, bool content_is_empty)
{
	std::map<int, std::map<std::string, Token>> tokens;
	tokens[NONE] = {
		{"em", Token("_", EM)},
		{"i",  Token("_", I)},
		{"p",  Token("\n", P, "", Token::TRUE)},
	};
	tokens[P] = {
		{"em", Token("_", EM)},
		{"i",  Token("_", I)},
		{"p",  Token("\n\n", P)},
		{"/p", Token("\n", NONE, "", Token::FALSE)},
	};
	tokens[EM] =  {
		{"/em", Token("", NONE, "_")},
		{"i",  Token("", I)},
		{"p",  Token("\n\n", P, "_", Token::TRUE)},
		{"/p", Token("\n", NONE, "", Token::FALSE)},
	};
	tokens[I] = {
		{"/i", Token("", NONE, "_")},
		{"em", Token("", EM)},
		{"p",  Token("\n\n", P, "_", Token::TRUE)},
		{"/p", Token("\n", NONE, "", Token::FALSE)},
	};
	if(tokens[mode].count(tag)) {
		const Token & token = tokens[mode].at(tag);
		if(!token.appendix_to_remove.empty()) {
			if(content_is_empty) {
				size_t length = token.appendix_to_remove.size();
				result.erase(result.size() - length, length);
			} else {
				result += token.appendix_to_remove;
			}
		}
		result += token.appendix;
		if(token.was_paragraph != Token::UNKNOWN) {
			was_paragraph = token.was_paragraph == Token::TRUE;
		}
		if(token.new_mode == NONE) {
			mode = was_paragraph ? P : NONE;
		} else {
			mode = token.new_mode;
		}
		return;
	}
}

void Html2MarkProcessor::process_parts()
{
	for(const ContentTag & content_tag : parts) {
		const std::string & content = content_tag.first;
		const std::string & tag = content_tag.second;

		result += content;
		process_tag(tag, content.empty());
	}
	if(was_paragraph) {
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
