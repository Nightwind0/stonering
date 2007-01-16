#include "SteelInterpreter.h"
#include "Ast.h"
#include "SteelParser.h"
#include "SteelFunctor.h"
#include "SteelException.h"
#include <sstream>
#include <iostream>
#include <string>

SteelInterpreter::SteelInterpreter()
{
    registerBifs();
    srand(time(0));
}

SteelInterpreter::~SteelInterpreter()
{
    // Delete functors
    for(std::map<std::string,SteelFunctor*>::iterator i = m_functions.begin();
	i != m_functions.end(); i++)
	delete i->second;
}


void SteelInterpreter::addFunction(const std::string &name,
				   SteelFunctor *pFunc)
{
    std::map<std::string,SteelFunctor*>::iterator it = m_functions.find ( name );

    if( it != m_functions.end() ) throw AlreadyDefined();

    m_functions[name] = pFunc;
}

void SteelInterpreter::run(const std::string &name,const std::string &script)
{
    SteelParser parser;
    parser.setBuffer(script.c_str(),name);
    if(parser.Parse() != SteelParser::PRC_SUCCESS)
    {
	AstBase * pAst = static_cast<AstBase*>( parser.GetAcceptedToken() );
	
	throw SteelException(SteelException::PARSING,
			     pAst->GetLine(),
			     pAst->GetScript(),
			     "Parse error.");
			     
    }

    AstScript *pScript = static_cast<AstScript*>( parser.GetAcceptedToken() );

    if(pScript->containsFunctions())
	pScript->registerFunctions(this);
    else pScript->executeScript(this);
    
    
}

SteelType SteelInterpreter::call(const std::string &name, const std::vector<SteelType> &pList)
{
    // First, check the builtins. They can be considered like keywords.

    std::map<std::string,SteelFunctor*>::iterator it = m_functions.find( name );

    if( it != m_functions.end() )
    {
	// Its found, and its a bif.
	SteelFunctor * pFunctor = it->second;
	assert ( pFunctor != NULL );

	return pFunctor->Call(this,pList);
    }
    else
    {

	throw UnknownIdentifier();
    }

    return SteelType();
}

void SteelInterpreter::setReturn(const SteelType &var)
{
    m_return = var;
}

SteelType SteelInterpreter::getReturn() const
{
    return m_return;
}


void SteelInterpreter::declare(const std::string &name)
{
    VariableFile &file = m_symbols.front();

    file[name] = SteelType();
}

// Note: Step 1 is to create a SteelArray in the ArrayFile
// is to create a SteelType in the variable file, and set it's 
// array reference to the array
void SteelInterpreter::declare_array(const std::string &array, int size)
{
    VariableFile &file = m_symbols.front();
    ArrayFile &array_file = m_arrays.front();

    SteelArrayRef ref; 
    std::string sref = name_array_ref( array );
    ref.setArrayRef ( sref );
    array_file[ref] = SteelArray( std::max(1,size) );
    
    // set the variable to reference it
    file[array].set ( ref );
}


SteelType SteelInterpreter::lookup(const std::string &name)
{
    // if strict, throw Unknown Identifier
    SteelType * pVar = lookup_internal(name);
    if(pVar == NULL) throw UnknownIdentifier();
    return *pVar;
}

SteelType SteelInterpreter::lookup(const std::string &array, int index)
{
    // if strict, throw Unknown Identifier
    SteelType *pVar = lookup_internal(array);
    if(pVar == NULL) throw UnknownIdentifier();

    SteelArray *pArray = lookup_internal ( (SteelArrayRef)*pVar);
    if(pArray == NULL) throw UnknownIdentifier();

    if(index >= pArray->size()) throw OutOfBounds();

    return (*pArray)[index];

}

void SteelInterpreter::assign(const std::string &name, const SteelType &value)
{
    // if strict, throw unknown id
    SteelType *pVar = lookup_internal(name);
    if(pVar == NULL) throw UnknownIdentifier();

    *pVar = value;
}

void SteelInterpreter::assign(const std::string &array, int index, const SteelType &value)
{
    // if strict, throw unknown id
    SteelType *pVar = lookup_internal(array);
    if(pVar == NULL) throw UnknownIdentifier();

    if(!pVar->isArray()) throw TypeMismatch();

    SteelArray *pArray = lookup_internal( (SteelArrayRef)*pVar );
    if(pArray == NULL) throw UnknownIdentifier();

    if(index >= pArray->size()) throw OutOfBounds();

    (*pArray)[index] = value;
}

std::string SteelInterpreter::name_array_ref(const std::string &array_name)
{
    std::ostringstream out;

    out << m_arrays.size() << array_name;

    return out.str();
}

void SteelInterpreter::assign_array(const std::string &name, const SteelType &value)
{
    if(!value.isArray())
	throw TypeMismatch();

    SteelType *pVar = lookup_internal( name );
    if(pVar == NULL) throw UnknownIdentifier();
    // Rely on cast overloader to SteelArrayRef
    SteelArray *pRhsArray = lookup_internal( (SteelArrayRef)value );
    if(pRhsArray == NULL) throw UnknownIdentifier();

    std::string newname = name_array_ref ( name );
    SteelArrayRef newref;
    newref.setArrayRef(newname);
    // Create a copy of actual array associated with value
    ArrayFile &file = m_arrays.front();
    // Insert copy into top-most file
    file[newref] = *pRhsArray ;
    // Assign ref to copy into pVar
    pVar->set(newref);
			     
}

void SteelInterpreter::registerFunction(const std::string &name, 
					AstParamDefinitionList *pParams, 
					AstStatementList *pStatements)
{
    addFunction(name,new SteelUserFunction( pParams, pStatements ));
}

SteelType * SteelInterpreter::lookup_internal(const std::string &name)
{
    for(std::list<VariableFile>::iterator i = m_symbols.begin();
	i != m_symbols.end(); i++)
    {
	VariableFile::iterator it = (*i).find(name);
	if( it != (*i).end()  )
	{
	    // Found a match.
	    return &(it->second);
	}
    }

    return NULL;
}

SteelInterpreter::SteelArray * SteelInterpreter::lookup_internal(const SteelArrayRef &ref)
{
    for(std::list<ArrayFile>::iterator i = m_arrays.begin();
	i != m_arrays.end(); i++)
    {
	ArrayFile::iterator it = (*i).find(ref);
	if( it != (*i).end() )
	{
	    return &(it->second);
	}
    }

    return NULL;
}


void SteelInterpreter::pushScope()
{
    m_symbols.push_front ( VariableFile() );
    m_arrays.push_front ( ArrayFile() );
}

void SteelInterpreter::popScope()
{
    m_symbols.pop_front();
    m_arrays.pop_front();
}



void SteelInterpreter::registerBifs()
{
    addFunction( "push", new SteelFunctor2Arg<SteelInterpreter,const SteelArrayRef &,const SteelType&> ( this, &SteelInterpreter::push ) );
    addFunction( "pop", new SteelFunctor1Arg<SteelInterpreter,const SteelArrayRef &>(this, &SteelInterpreter::pop) );
    addFunction( "print", new SteelFunctor1Arg<SteelInterpreter,const std::string &>(this, &SteelInterpreter::print ) );
    addFunction( "println", new SteelFunctor1Arg<SteelInterpreter,const std::string &>(this,&SteelInterpreter::println ) );
    addFunction( "len", new SteelFunctor1Arg<SteelInterpreter,const SteelArrayRef&>(this, &SteelInterpreter::len ) );
    addFunction( "copy", new SteelFunctor2Arg<SteelInterpreter,const SteelArrayRef&,const SteelArrayRef&> ( this, &SteelInterpreter::copy ) );
    addFunction( "real", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::real ) );
    addFunction( "integer", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::integer ) );
    addFunction( "boolean", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::boolean ) );
    addFunction( "substr", new SteelFunctor3Arg<SteelInterpreter,const std::string&,int, int>(this,&SteelInterpreter::substr ) );
}

SteelType SteelInterpreter::push(const SteelArrayRef &ref, const SteelType &value)
{
    SteelArray *pArray = lookup_internal(ref);

    if(pArray == NULL) throw UnknownIdentifier();

    pArray->push_back ( value );

    return pArray->back();
					    
}

SteelType SteelInterpreter::pop(const SteelArrayRef &ref)
{
    SteelArray *pArray = lookup_internal(ref);


    if(pArray == NULL) throw UnknownIdentifier();

    SteelType val = pArray->front();
    pArray->pop_back();
    
    return val;
}

SteelType SteelInterpreter::print(const std::string &str)
{
    std::cout << str;
    SteelType var;
    var.set(str);

    return var;
}

SteelType SteelInterpreter::println(const std::string &str)
{
    std::cout << str << std::endl;
    SteelType var;
    var.set(str);

    return var;
}

SteelType SteelInterpreter::len(const SteelArrayRef &ref)
{
    SteelArray *pArray = lookup_internal(ref);

    if(pArray == NULL) throw UnknownIdentifier();

    SteelType val;

    val.set( (int)pArray->size() );

    return val;
}

SteelType SteelInterpreter::copy(const SteelArrayRef &lhs, const SteelArrayRef &rhs)
{
    SteelType ret;
    ret.set ( rhs );
    SteelArray *pArrayDest = lookup_internal(lhs);
    if(pArrayDest == NULL) throw UnknownIdentifier();
    SteelArray *pArraySource = lookup_internal(rhs);
    if(pArraySource == NULL) throw UnknownIdentifier();

    // They both point to the same thing already
    if(pArrayDest == pArraySource) return ret;

    *pArrayDest = *pArraySource;
    
}

SteelType SteelInterpreter::real(const SteelType &value)
{
    SteelType var;
    var.set ( (double)value );

    return var;
}

SteelType SteelInterpreter::integer(const SteelType &value)
{
    SteelType var;
    var.set ( (int)value );

    return var;
}

SteelType SteelInterpreter::boolean(const SteelType &value)
{
    SteelType var;
    var.set ( (bool)value );

    return var;
}

SteelType SteelInterpreter::substr(const std::string &str, int start, int len)
{
    SteelType var;
    std::string value;

    value = str.substr (start, len);
    var.set ( value );

    return var;
}
