#pragma once
#include <string>
#include <map>

struct XMLReader {
	XMLReader(std::istream & stream);

	const std::string & to_next_tag();
	const std::string & skip_to_tag(const std::string & tag_name);

	const std::string & get_current_tag() const { return current_tag; }
	const std::string & get_current_content() const { return current_content; }
	std::map<std::string, std::string> get_attributes() const { return attributes; }
private:
	std::istream & s;
	std::string current_tag;
	std::string current_content;
	std::map<std::string, std::string> attributes;
};

