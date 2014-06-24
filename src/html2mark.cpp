#include "html2mark.h"
#include "xmlreader.h"
#include <chthon2/log.h>
#include <sstream>
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

std::string html2mark(const std::string & html, int /*options*/,
		int /*min_reference_links_length*/)
{
	std::string result;
	std::istringstream stream(html);
	XMLReader reader(stream);
	reader.to_next_tag();
	std::string content = collapse(reader.get_current_content(), true, true);
	result += content;

	return result;
}

}
