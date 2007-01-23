#ifndef SR_STEEL_INTERPRETER_H
#define SR_STEEL_INTERPRETER_H

#include <map>
#include <string>
#include <list>
#include <vector>
#include "SteelFunctor.h"

// fwd
class AstStatementList; 
class AstParamDefinitionList;

class SteelInterpreter
{
public:
    SteelInterpreter();
    virtual ~SteelInterpreter();

    void addFunction(const std::string &name, SteelFunctor *pFunc);
    SteelType call(const std::string &name, const std::vector<SteelType> &pList);
    void run(const std::string &name,const std::string &script);

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
