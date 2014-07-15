#include "src/html2mark.h"
#include <getopt.h>
#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
	int options = Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS;
	std::string filename;

	static struct option long_options[] = {
		{"color", no_argument, nullptr, 'c'},
		{nullptr, 0, nullptr, 0}
	};
	while(true) {
		int option_index = 0;
		int c = getopt_long(argc, argv, "c", long_options, &option_index);
		if(c == -1) {
			break;
		}
		switch(c) {
			case 'c': options |= Html2Mark::COLORS; break;
			case '?': break;
			default: return 1;
		}
	}
	if(optind < argc) {
		filename = argv[optind];
	}

	if(!filename.empty()) {
		std::ifstream in(filename);
		if(in.fail()) {
			std::cerr << "Cannot open file \"" << filename << "\"!" << std::endl;
			return 1;
		}
		std::cout << Html2Mark::html2mark(in, options);
	} else {
		std::cout << Html2Mark::html2mark(std::cin, options);
	}
	return 0;
}
