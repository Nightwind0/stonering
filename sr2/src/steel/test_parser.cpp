#include <iostream>
#include "SteelParser.h"

int main()
{
    SteelParser parser;

    parser.SetDebugSpewLevel(2);
    if(parser.Parse() != SteelParser::PRC_SUCCESS)
    {
	std::cout << "Parse error unknown." << std::endl;
	return 1;
    }

    return 0;
}
