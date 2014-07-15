#include "src/html2mark.h"
#include <getopt.h>
#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
	int options = Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS;
	size_t wrap_width = 0;
	std::string filename;

	static struct option long_options[] = {
		{"color", no_argument, nullptr, 'c'},
		{"width", required_argument, nullptr, 'w'},
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
			case 'w': {
				wrap_width = strtoul(optarg, nullptr, 10);
				if(wrap_width <= 0) {
					std::cerr << "Width must be greater than 0.\n";
					return 1;
				}
				options |= Html2Mark::WRAP;
				break;
			}
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
		std::cout << Html2Mark::html2mark(in, options, 20, wrap_width);
	} else {
		std::cout << Html2Mark::html2mark(std::cin, options, 20, wrap_width);
	}
	return 0;
}
