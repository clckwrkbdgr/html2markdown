#include "src/html2mark.h"
#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
	if(argc > 1) {
		std::ifstream in(argv[1]);
		if(in.fail()) {
			std::cerr << "Cannot open file \"" << argv[1] << "\"!" << std::endl;
			return 1;
		}
		std::cout << Html2Mark::html2mark(in,
				Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS);
	} else {
		std::cout << Html2Mark::html2mark(std::cin,
				Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS);
	}
	return 0;
}
