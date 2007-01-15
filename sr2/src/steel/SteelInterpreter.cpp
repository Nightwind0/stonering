#include "SteelInterpreter.h"
#include "Ast.h"
#include "SteelParser.h"
#include "SteelFunctor.h"
#include <sstream>

SteelInterpreter::SteelInterpreter()
{
}

SteelInterpreter::~SteelInterpreter()
{
    // Delete functors
    for(std::map<std::string,SteelFunctor*>::iterator i = m_bifs.begin();
	i != m_bifs.end(); i++)
	delete i->second;
}


void SteelInterpreter::addFunction(const std::string &name,
				   SteelFunctor *pFunc)
{
    m_bifs[name] = pFunc;
}

void SteelInterpreter::run(const std::string &name,const std::string &script)
{
    SteelParser parser;
    parser.setBuffer(script.c_str(),name);
    if(parser.Parse() != SteelParser::PRC_SUCCESS)
    {
	std::cerr << "Parse error." << std::endl;
	return;
    }

    AstScript *pScript = static_cast<AstScript*>( parser.GetAcceptedToken() );

    
}


void SteelInterpreter::setReturn(const SteelType &var)
{
    m_return = var;
}

SteelType SteelInterpreter::getReturn() const
{
    return m_return;
}



SteelType SteelInterpreter::lookup(const std::string &name)
{
    // if strict, throw Unknown Identifier
}

SteelType SteelInterpreter::lookup(const std::string &array, int index)
{
    // if strict, throw Unknown Identifier
}

void SteelInterpreter::assign(const std::string &name, const SteelType &value)
{
    // if strict, throw unknown id
}

void SteelInterpreter::assign(const std::string &array, int index, const SteelType &value)
{
    // if strict, throw unknown id
}

std::string SteelInterpreter::name_array_ref(const std::string &array_name)
{
    std::ostringstream out;

    out << m_symbols.size() << array_name;

    return out.str();
}

void SteelInterpreter::assign_array(const std::string &name, const SteelType &value)
{
}

void SteelInterpreter::augment_array(const std::string &name, const SteelType &value)
{
}

void SteelInterpreter::registerFunction(const std::string &name, 
					AstParamDefinitionList *pParams, 
					AstStatementList *pStatements)
{
    
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

SteelType * SteelInterpreter::lookup_array_internal(const std::string &name)
{
    
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
}

SteelType SteelInterpreter::push(const SteelArrayRef &ref, const SteelType &value)
{
}

SteelType SteelInterpreter::pop(const SteelArrayRef &ref)
{
}
