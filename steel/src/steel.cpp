#include <iostream>
#include "SteelInterpreter.h"
#include "SteelException.h"

int main(int argc, char * argv[])
{
    SteelInterpreter interpreter;

    std::string script;

    bool bPrint=false;
    bool bRun=true;

    for(int i=0;i<argc;i++)
    {
        std::string arg = argv[i];

        if(arg == "--print" ||
           arg == "-p")
        {
            bPrint = true;
        }
        else if(arg == "--no-exec" ||
                arg == "-n")
        {
            bRun = false;
        }
        
    }


    while(!std::cin.eof())
    {
        script += std::cin.get();
    }

    try{
        AstScript * pScript = interpreter.prebuildAst("<STDIN>",script);

        if(bPrint)
            pScript->print(std::cout);

        if(bRun)
            interpreter.runAst(pScript);
    }
    catch(SteelException ex)
    {
        if(ex.getType() != SteelException::PARSING)
            std::cerr << ex.getLine() << ':';
        
        std::cerr << ex.getMessage() << std::endl;
    }
}

