#include <iostream>
#include "SteelInterpreter.h"
#include "SteelException.h"

int main()
{
    SteelInterpreter interpreter;

    std::string script;

    while(!std::cin.eof())
    {
	script += std::cin.get();
    }

    try{
	interpreter.run("STDIO", script);
    }
    catch(SteelException ex)
    {
	std::cerr << "Error on line " << ex.getLine() << ": " << ex.getMessage() << std::endl;
    }
}
