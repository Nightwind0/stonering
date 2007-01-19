#include "SteelInterpreter.h"
#include "Ast.h"
#include "SteelParser.h"
#include "SteelFunctor.h"
#include "SteelException.h"
#include <ctime>
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
    // Since the user defined functions are in the main statement list,
    // they are already deleted when you delete the tree... therefore,
    // we don't want to delete them here.
    for(std::map<std::string,SteelFunctor*>::iterator i = m_functions.begin();
	i != m_functions.end(); i++)
    {
	delete i->second;
    }
	    
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

	if( pAst != NULL)
	{
	    throw SteelException(SteelException::PARSING,
				 pAst->GetLine(),
				 pAst->GetScript(),
				 "Parse error.");
	}
	else
	{
	    // Apparently, there was nothing there. 
	    // Which should be legal.
	    // And theres nothing to delete. So. I think we're done here.
	    return;
				 
	}
			     
    }

    AstScript *pScript = static_cast<AstScript*>( parser.GetAcceptedToken() );


    pScript->executeScript(this);
    
    delete pScript;
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
void SteelInterpreter::declare_array(const std::string &array_name, int size)
{
    VariableFile &file = m_symbols.front();
    SteelType var;
    var.set ( SteelArray( std::max(size,0)  ) );
    
    file[array_name]  = var;
}


SteelType *SteelInterpreter::lookup_lvalue(const std::string &name)
{
    SteelType *p = lookup_internal(name);

    if(p == NULL) throw UnknownIdentifier();
    return p;
}



SteelType SteelInterpreter::lookup(const std::string &name)
{
    // if strict, throw Unknown Identifier
    SteelType * pVar = lookup_internal(name);
    if(pVar == NULL) throw UnknownIdentifier();
    return *pVar;
}

SteelType SteelInterpreter::lookup(SteelType *pVar, int index)
{
    // if strict, throw Unknown Identifier
    // TODO: Unknown ID, or was it not an lvalue
    if(pVar == NULL) throw UnknownIdentifier();
    if(!pVar->isArray()) throw TypeMismatch();
  
    return pVar->getElement(index);
}

void SteelInterpreter::assign(SteelType *pVar, const SteelType &value)
{
    // if strict, throw unknown id
    if(pVar == NULL) throw UnknownIdentifier();

    *pVar = value;
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


void SteelInterpreter::pushScope()
{
    m_symbols.push_front ( VariableFile() );
}

void SteelInterpreter::popScope()
{
    m_symbols.pop_front();
}



void SteelInterpreter::registerBifs()
{
//    addFunction( "push", new SteelFunctor2Arg<SteelInterpreter,const SteelArrayRef &,const SteelType&> ( this, &SteelInterpreter::push ) );
//    addFunction( "pop", new SteelFunctor1Arg<SteelInterpreter,const SteelArrayRef &>(this, &SteelInterpreter::pop) );
    addFunction( "print", new SteelFunctor1Arg<SteelInterpreter,const std::string &>(this, &SteelInterpreter::print ) );
    addFunction( "println", new SteelFunctor1Arg<SteelInterpreter,const std::string &>(this,&SteelInterpreter::println ) );
    addFunction( "len", new SteelFunctor1Arg<SteelInterpreter,const SteelArray&>(this, &SteelInterpreter::len ) );
    addFunction( "real", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::real ) );
    addFunction( "integer", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::integer ) );
    addFunction( "boolean", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::boolean ) );
    addFunction( "substr", new SteelFunctor3Arg<SteelInterpreter,const std::string&,int, int>(this,&SteelInterpreter::substr ) );
    addFunction( "strlen", new SteelFunctor1Arg<SteelInterpreter,const std::string&>(this,&SteelInterpreter::strlen));
    addFunction( "is_array", new SteelFunctor1Arg<SteelInterpreter,const SteelType&>(this,&SteelInterpreter::is_array));
}

/*
SteelType SteelInterpreter::push(const SteelArrayRef &ref, const SteelType &value)
{
    SteelArray *pArray = lookup_internal(ref);

    if(pArray == NULL) throw UnknownIdentifier();

    pArray->push_front ( value );

    return pArray->front();
					    
}
*/

/*

SteelType SteelInterpreter::pop(const SteelArrayRef &ref)
{
    SteelArray *pArray = lookup_internal(ref);


    if(pArray == NULL) throw UnknownIdentifier();

    SteelType val = pArray->front();
    pArray->pop_front();
    
    return val;
}
*/

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

SteelType SteelInterpreter::len(const SteelArray &array)
{
    SteelType val;

    val.set((int)array.size());

    return val;
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

SteelType SteelInterpreter::strlen(const std::string &str)
{
    SteelType var;

    var.set ( (int)str.size() );

    return var;
}

SteelType SteelInterpreter::is_array(const SteelType &array)
{
    SteelType var;
    var.set ( array.isArray() );

    return var;
}
