#include <string>
#include <queue>
#include "SteelFunctor.h"
#include "SteelInterpreter.h"
#include "Ast.h"

SteelFunctor::SteelFunctor()
{
}

SteelFunctor::~SteelFunctor()
{
}


SteelUserFunction::SteelUserFunction(AstParamDefinitionList *pParams, AstStatementList *pList)
    :m_pParams(pParams),m_pList(pList)
{
}

SteelUserFunction::~SteelUserFunction()
{
    delete m_pParams;
    delete m_pList;
}

SteelType SteelUserFunction::Call(SteelInterpreter * pInterpreter,const std::vector<SteelType> &params)
{
    SteelType ret;
    pInterpreter->pushScope();

    if( m_pParams != NULL)
    {
	if( params.size() != m_pParams->size() ) throw ParamMismatch();
    }
    else 
    {
	if( params.size() != 0) throw ParamMismatch();
    }

    if(m_pParams)
	m_pParams->executeDeclarations(pInterpreter, params);

    if(m_pList)
    {
	m_pList->execute(pInterpreter);
	ret = pInterpreter->getReturn();
    }

    pInterpreter->popScope();
    return ret;
}


void SteelUserFunction::setParamDefinitionList(AstParamDefinitionList *pParams)
{
    m_pParams = pParams;
}

void SteelUserFunction::setStatementList(AstStatementList *pList)
{
    m_pList = pList;
}
