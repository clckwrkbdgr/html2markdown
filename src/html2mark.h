#pragma once
#include <string>
#include <istream>

namespace Html2Mark {

enum {
	DEFAULT_OPTIONS = 0x00,
	UNDERSCORED_HEADINGS = 0x01,
	MAKE_REFERENCE_LINKS = 0x02,
	COUNT = 0x100
};

std::string html2mark(const std::string & html, int options = DEFAULT_OPTIONS,
		int min_reference_links_length = 20);
std::string html2mark(std::istream & input, int options = DEFAULT_OPTIONS,
		int min_reference_links_length = 20);

}
