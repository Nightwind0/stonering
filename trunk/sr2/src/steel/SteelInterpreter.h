#ifndef SR_STEEL_INTERPRETER_H
#define SR_STEEL_INTERPRETER_H

#include <map>
#include <string>
#include <list>
#include <vector>
#include "SteelParser.h"
#include "SteelFunctor.h"

// fwd
class AstScript;
class AstStatementList; 
class AstParamDefinitionList;

class SteelInterpreter
{
public:
    SteelInterpreter();
    virtual ~SteelInterpreter();

    void addFunction(const std::string &name, SteelFunctor *pFunc);
    SteelType call(const std::string &name, const std::vector<SteelType> &pList);

    // This allows you to pre-parse a script and keep a pointer to it
    // around, which can be run over and over, and the deletion of it is
    // up to the user of SteelInterpreter
    AstScript * prebuildAst(const std::string &script_name,
                            const std::string &script);

    // After using prebuildAst, you can later run it using runAst
    SteelType runAst(AstScript *pAst );
    SteelType run(const std::string &name,const std::string &script);

    SteelType lookup(const std::string &name);
    // Array element lookup
    SteelType lookup(SteelType *pVar, int index);
    SteelType *lookup_lvalue(const std::string &name);
    void declare(const std::string &name);
    void declare_array(const std::string &array, int size);

    void assign(SteelType *pVar,const SteelType &value);

    void pushScope();
    void popScope();
    void registerFunction(const std::string &name, 
                          AstParamDefinitionList *pParams, 
                          AstStatementList *pStatements);
    void setReturn(const SteelType &var);
    SteelType getReturn() const;

private:

    std::string name_array_ref(const std::string &array_name); 
    typedef std::map<std::string, SteelType> VariableFile;
    std::list<VariableFile> m_symbols;
  

    SteelType * lookup_internal(const std::string &name);
  
    void registerBifs();
    std::map<std::string,SteelFunctor*> m_functions;

    SteelType m_return;

private:
    // Bifs
    // Push on the FRONT of the array
//    SteelType push(const SteelArrayRef &ref, const SteelType &value);
    // Pop the front of the array
//    SteelType pop(const SteelArrayRef &ref);
    // Pop the back of the array
//    SteelType push(const SteelArray &ref, const SteelType &rhs);
    // Push on the BACK of the array
    SteelType print(const std::string &str);
    SteelType println(const std::string &str);
    SteelType len(const SteelArray &ref);
//    SteelType copy(const SteelArray &lhs, const SteelArrayRef &rhs);
    SteelType real(const SteelType &str);
    SteelType integer(const SteelType &str);
    SteelType boolean(const SteelType &str);
    SteelType substr(const std::string &str, int start, int len);
    SteelType strlen(const std::string &str);
    SteelType is_array(const SteelType &);
};


#endif
