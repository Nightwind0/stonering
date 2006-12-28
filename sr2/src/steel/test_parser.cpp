#include <iostream>
//#include <ifstream>
#include "SteelParser.h"

int main()
{
    SteelParser parser;
    std::string script;

    while(!std::cin.eof())
    {
	script += std::cin.get();
    }

    std::cout << "STEEL SCRIPT: [[" << std::endl;
    std::cout << script << "]]" << std::endl;


    parser.SetDebugSpewLevel(2);
    parser.setBuffer( script.c_str() , "Magic Script");
    if(parser.Parse() != SteelParser::PRC_SUCCESS)
    {
	std::cout << "Parse error unknown." << std::endl;
	return 1;
    }

    return 0;
}
