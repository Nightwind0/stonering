#ifndef SR_STEEL_INTERPRETER_H
#define SR_STEEL_INTERPRETER_H

#include <map>
#include <string>
#include <list>
#include "SteelFunctor.h"

// fwd
class AstStatementList; 

class SteelInterpreter
{
public:
    SteelInterpreter();
    virtual ~SteelInterpreter();

    void addFunction(const std::string &name, SteelFunctor *pFunc);
    SteelType call(const std::string &name, ParamList &pList);
    void run(const std::string &name,const std::string &script);

    SteelType lookup(const std::string &name);
    SteelType lookup(const std::string &array, int index);
    void declare(const std::string &name);
    void declare_array(const std::string &array, int size);
    void assign(const std::string &name, const SteelType &value);
    void assign(const std::string &array, int index, const SteelType &value);
    void assign_array(const std::string &name, const SteelType &value);
    void augment_array(const std::string &name, const SteelType &value);
    std::string name_array_ref(const std::string &array_name); 
    void pushScope();
    void popScope();
    void registerFunction(const std::string &name, 
			  const std::list<std::string> &params, 
			  AstStatementList *pStatements);
    void setReturn(const SteelType &var);
    SteelType getReturn() const;

private:


    typedef std::map<std::string, SteelType> VariableFile;
    typedef std::vector<SteelType> SteelArray;
    
    std::list<VariableFile> m_symbols;
    std::map<std::string,SteelArray> m_arrays;

    SteelType * lookup_internal(const std::string &name);
    SteelType * lookup_array_internal(const std::string &name);

    void registerBifs();
    std::map<std::string,SteelFunctor*> m_bifs;
    std::map<std::string, AstStatementList*> m_functions;
    SteelType m_return;

private:
    // Bifs
    SteelType push(const SteelArrayRef &ref, const SteelType &value);
    SteelType pop(const SteelArrayRef &ref);
};


#endif
