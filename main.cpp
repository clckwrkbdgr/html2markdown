#include "src/html2mark.h"
#include <iostream>

int main()
{
	std::cout << Html2Mark::html2mark(std::cin,
			Html2Mark::UNDERSCORED_HEADINGS | Html2Mark::MAKE_REFERENCE_LINKS);
	return 0;
}
