#ifndef SR_STEEL_INTERPRETER_H
#define SR_STEEL_INTERPRETER_H

#include <map>
#include <string>
#include <list>
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
    SteelType call(const std::string &name, ParamList &pList);
    void run(const std::string &name,const std::string &script);

    SteelType lookup(const std::string &name);
    // Array element lookup
    SteelType lookup(const std::string &array, int index);
    void declare(const std::string &name);

    // Note: Step 1 is to create a SteelArray in the ArrayFile
    // is to create a SteelType in the variable file, and set it's 
    // array reference to the array
    void declare_array(const std::string &array, int size);

    // Note: Runtime check that we aren't trying to assign an array
    // to a var. throw TypeMismatch
    void assign(const std::string &name, const SteelType &value);
    void assign(const std::string &array, int index, const SteelType &value);

    // Note: Runtime check that value is of type array
    // or throw TypeMismatch
    void assign_array(const std::string &name, const SteelType &value);

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
    typedef std::vector<SteelType> SteelArray;
    typedef std::map<SteelArrayRef,SteelArray> ArrayFile;
    
    std::list<VariableFile> m_symbols;
    std::list<ArrayFile> m_arrays;

    SteelType * lookup_internal(const std::string &name);
    SteelArray * lookup_internal(const SteelArrayRef &ref);

    void registerBifs();
    std::map<std::string,SteelFunctor*> m_bifs;
    std::map<std::string, AstStatementList*> m_functions;
    SteelType m_return;

private:
    // Bifs
    SteelType push(const SteelArrayRef &ref, const SteelType &value);
    SteelType pop(const SteelArrayRef &ref);
    SteelType print(const std::string &str);
    SteelType len(const SteelArrayRef &ref);
};


#endif
