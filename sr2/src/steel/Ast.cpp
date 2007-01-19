#include "Ast.h"
#include "SteelInterpreter.h"
#include "SteelType.h"
#include "SteelException.h"
#include <string>
#include <iostream>
#include <cassert>


ostream & operator<<(ostream &,AstBase&);


AstBase::AstBase(unsigned int line, const std::string &script)
    :m_line(line),m_script_name(script)
{
}

AstBase::~AstBase()
{
}



AstInteger::AstInteger(unsigned int line,
		       const std::string &script,
		       int value)
    :AstExpression(line,script),m_value(value)
{
}

ostream & AstInteger::print(std::ostream &out)
{
    out << m_value;
    return out;
}

SteelType AstInteger::evaluate(SteelInterpreter *pInterpreter)
{
    SteelType val;
    val.set(m_value);
    return val;
}

AstString::AstString(unsigned int line,
		     const std::string &script)
    : AstExpression(line,script)
{
}


void AstString::addChar(const char c)
{
    m_value += c;
}

void AstString::addString(const std::string &str)
{
    m_value += str;
}

ostream & AstString::print(std::ostream &out)
{
    out << '\"' << m_value << '\"';
    return out;
}

std::string AstString::translate_escapes()
{
    
}

SteelType AstString::evaluate(SteelInterpreter *pInterpreter)
{
    SteelType var;
    var.set(m_value);

    return var;
}


AstFloat::AstFloat(unsigned int line,
		   const std::string &script,
		   double value)
    : AstExpression(line,script),m_value(value)
{
    
}

ostream & AstFloat::print(std::ostream &out)
{
    out << m_value;
    return out;
}

SteelType AstFloat::evaluate(SteelInterpreter *pInterpreter)
{
    SteelType var;
    var.set(m_value);
    
    return var;
}

AstIdentifier::AstIdentifier(unsigned int line,
			     const std::string &script,
			     const std::string &value)
    : AstExpression(line,script),m_value(value)
{
    
}

ostream & AstIdentifier::print(std::ostream &out)
{
    out << m_value;
    return out;
}

SteelType AstIdentifier::evaluate(SteelInterpreter *pInterpreter)
{
    // Shouldn't ever be called.
    return SteelType();
}


AstStatement::AstStatement(unsigned int line, const std::string &script)
    :AstBase(line,script)
{
}

AstStatement::~AstStatement()
{
}

ostream & AstStatement::print(std::ostream &out)
{
    out << ";\n";
    return out;
}


AstExpressionStatement::AstExpressionStatement(unsigned int line, const std::string &script, AstExpression *pExp)
    :AstStatement(line,script),m_pExp(pExp)
{
    assert ( pExp );
}
AstExpressionStatement::~AstExpressionStatement()
{
    delete m_pExp;
}

ostream & AstExpressionStatement::print(std::ostream &out)
{
    out << *m_pExp << ';' << std::endl;
	return out;
}

AstStatement::eStopType AstExpressionStatement::execute(SteelInterpreter *pInterpreter)
{
    m_pExp->evaluate(pInterpreter);
    return AstStatement::COMPLETED;
}

AstScript::AstScript(unsigned int line, const std::string &script)
    :AstBase(line,script),m_pList(NULL)
{
}
AstScript::~AstScript()
{
    delete m_pList;
}

ostream & AstScript::print(std::ostream &out)
{
    if(m_pList) out << *m_pList;

    return out;
}

void AstScript::executeScript(SteelInterpreter *pInterpreter)
{
    if(m_pList)
    {
	m_pList->execute(pInterpreter);
    }
}

void AstScript::SetList(AstStatementList *pList)
{
    m_pList = pList;
}


AstStatementList::AstStatementList(unsigned int line, const std::string &script)
    :AstStatement(line,script)
{
}

AstStatementList::~AstStatementList()
{
    for(std::list<AstStatement*>::iterator i = m_list.begin();
	i != m_list.end(); i++) delete *i;
}

AstStatement::eStopType AstStatementList::execute(SteelInterpreter *pInterpreter)
{
    eStopType ret = COMPLETED;
    pInterpreter->pushScope();
    for(std::list<AstStatement*>::const_iterator it = m_list.begin();
	it != m_list.end(); it++)
    {
	
	AstStatement * pStatement = *it;
	eStopType stop = pStatement->execute(pInterpreter);

	if(stop == BREAK || stop == RETURN || stop ==  CONTINUE)
	{
	    ret = stop;
	    break;
	}
    }
    pInterpreter->popScope();

    return ret;
}

ostream & AstStatementList::print(std::ostream &out)
{
    out << "{\n";
    for(std::list<AstStatement*>::const_iterator it = m_list.begin();
	it != m_list.end(); it++)
    {
	AstStatement * pStatement = *it;
	out << "\t\t" << *pStatement;
    }
    out << "\t}\n";

    return out;
}


AstWhileStatement::AstWhileStatement(unsigned int line, const std::string &script, AstExpression *pExp, AstStatement *pStmt)
    :AstStatement(line,script),m_pCondition(pExp),m_pStatement(pStmt)
{
}
AstWhileStatement::~AstWhileStatement()
{
    delete m_pCondition;
    delete m_pStatement;
}

AstStatement::eStopType AstWhileStatement::execute(SteelInterpreter *pInterpreter)
{
    // I expect it to cast to boolean
    while( m_pCondition->evaluate(pInterpreter) )
    {
	eStopType stop = m_pStatement->execute(pInterpreter);

	if(stop == BREAK) return COMPLETED;
	else if(stop == RETURN) return RETURN;

	// Note: if stop is CONTINUE,
	// Then we just want to keep looping. So no action.
    }

	return COMPLETED;
}

ostream & AstWhileStatement::print(std::ostream &out)
{
    out << "while (" << *m_pCondition << ")\n"
	<< '\t' << *m_pStatement<< std::endl;

    return out;
}

AstIfStatement::AstIfStatement(unsigned int line, const std::string &script,
			       AstExpression *pExp, AstStatement *pStmt,
			       AstStatement *pElse)
    :AstStatement(line,script),m_pCondition(pExp),m_pStatement(pStmt),m_pElse(pElse)
{
}
AstIfStatement::~AstIfStatement()
{
    delete m_pCondition;
    delete m_pElse;
    delete m_pStatement;
}

AstStatement::eStopType AstIfStatement::execute(SteelInterpreter *pInterpreter)
{
    if(m_pCondition->evaluate(pInterpreter))
	return m_pStatement->execute(pInterpreter);
    else if ( m_pElse) return m_pElse->execute(pInterpreter);
    else return COMPLETED;

}

ostream & AstIfStatement::print(std::ostream &out)
{
    out << "if (" << *m_pCondition  << ")\n"
	<< '\t' << *m_pStatement << '\n';
    if(m_pElse) 
    {
	out << " else " << *m_pElse << '\n';
    }

    return out;
}

AstReturnStatement::AstReturnStatement(unsigned int line, const std::string &script, AstExpression *pExp)
    :AstStatement(line,script),m_pExpression(pExp)
{
}

AstReturnStatement::~AstReturnStatement()
{
    delete m_pExpression;
}

AstStatement::eStopType AstReturnStatement::execute(SteelInterpreter *pInterpreter)
{
    if( m_pExpression )
	pInterpreter->setReturn( m_pExpression->evaluate(pInterpreter) );
    else pInterpreter->setReturn ( SteelType() );
    return RETURN;
}

ostream & AstReturnStatement::print(std::ostream &out)
{
    out << "return ";
    if(m_pExpression)
    {
	out << '(' << *m_pExpression << ')';
    }
    out << ';' << std::endl;
    return out;

}

AstLoopStatement::AstLoopStatement(unsigned int line, const std::string &script,
				   AstExpression *pStart, AstExpression *pCondition,
				   AstExpression *pIteration, AstStatement* pStmt)
    :AstStatement(line,script),m_pStart(pStart),m_pCondition(pCondition),m_pIteration(pIteration),m_pStatement(pStmt)
{
}

AstLoopStatement::~AstLoopStatement()
{
    delete m_pStart;
    delete m_pCondition;
    delete m_pIteration;
    delete m_pStatement;
}

AstStatement::eStopType AstLoopStatement::execute(SteelInterpreter *pInterpreter)
{
    for( m_pStart->evaluate( pInterpreter) ;
	 m_pCondition->evaluate(pInterpreter) ;
	 m_pIteration->evaluate( pInterpreter )
	)
    {
	eStopType stop = m_pStatement->execute(pInterpreter);

	if(stop == BREAK) return COMPLETED;
	else if (stop == RETURN) return RETURN;

	// For both CONTINUE and COMPLETED, we just keep going.
    }

    return COMPLETED;
}

ostream & AstLoopStatement::print(std::ostream &out)
{
    out << "for (" << *m_pStart << ';' << *m_pCondition << ';' << *m_pIteration << ')' << *m_pStatement;
    
    return out;
}





AstBinOp::AstBinOp(unsigned int line,
		   const std::string &script,
		   Op op, AstExpression *left, AstExpression *right)
    :AstExpression(line,script),m_op(op),m_right(right),m_left(left)
{
}
AstBinOp::~AstBinOp()
{
    delete m_right;
    delete m_left;
}

std::string AstBinOp::ToString(Op op)
{
    switch(op)
    {
    case ADD:
	return "+";
    case SUB:
	return "-";
    case MULT:
	return "*";
    case DIV:
	return "/";
    case MOD:
	return "%";
    case AND:
	return " and ";
    case OR:
	return " or ";
    case D:
	return " d ";
    case POW:
	return "^";
    case EQ:
	return " == ";
    case NE:
	return " != ";
    case LT:
	return " < ";
    case GT:
	return " > ";
    case LTE:
	return " <= ";
    case GTE:
	return " >=";
    default:
	assert(0);
	return "";
    }
}

SteelType *AstBinOp::lvalue(SteelInterpreter *pInterpreter)
{
    return NULL;
}

SteelType AstBinOp::evaluate(SteelInterpreter *pInterpreter)
{
    try{

	switch( m_op )
	{
	case ADD:

	    return m_left->evaluate(pInterpreter) 
		+ m_right->evaluate(pInterpreter);
	    
	case SUB:
	    return m_left->evaluate(pInterpreter) 
		- m_right->evaluate(pInterpreter);
	case MULT:
	    return m_left->evaluate(pInterpreter) 
		* m_right->evaluate(pInterpreter);
	case DIV:
	    return m_left->evaluate(pInterpreter)
		/ m_right->evaluate(pInterpreter);
	case MOD:
	    return m_left->evaluate(pInterpreter)
		% m_right->evaluate(pInterpreter);
	case AND:
	{
	    SteelType var;
	    var.set((bool)m_left->evaluate(pInterpreter)
		    && (bool)m_right->evaluate(pInterpreter));

	    return var;
	}
	case OR:
	{
	    SteelType var;
	    var.set((bool)m_left->evaluate(pInterpreter)
		    || (bool)m_right->evaluate(pInterpreter));

	    return var;
	}
	case D:
	    return m_left->evaluate(pInterpreter).d( m_right->evaluate(pInterpreter));
	case POW:
	    return m_left->evaluate(pInterpreter)
		^ m_right->evaluate(pInterpreter);
	case EQ:
	{
	    SteelType var;
	    var.set( m_left->evaluate(pInterpreter)
		     == m_right->evaluate(pInterpreter));
	    return var;
	}
	case NE:
	{
	    SteelType var;
	    var.set ( m_left->evaluate(pInterpreter)
		      != m_right->evaluate(pInterpreter));
	    return var;
	}
	case LT:
	    return m_left->evaluate(pInterpreter)
		< m_right->evaluate(pInterpreter);
	case GT:
	    return m_left->evaluate(pInterpreter)
		> m_right->evaluate(pInterpreter);
	case LTE:
	    return m_left->evaluate(pInterpreter)
		<= m_right->evaluate(pInterpreter);
	case GTE:
	    return m_left->evaluate(pInterpreter)
		>= m_right->evaluate(pInterpreter);
	default:
	    assert(0);
	
	}
    }
    catch(OperationMismatch m)
    {
	throw SteelException(SteelException::RUNTIME,
			     GetLine(),GetScript(),
			     "Binary operation '" + ToString(m_op) + "' disallowed between these types");
    }

    return SteelType();
}

ostream & AstBinOp::print(std::ostream &out)
{
    out << *m_left <<  ToString(m_op) << *m_right;

    return out;
}

AstUnaryOp::AstUnaryOp(unsigned int line,
		       const std::string &script, Op op,
		       AstExpression *operand)
    :AstExpression(line,script),m_op(op),m_operand(operand)
{

}

AstUnaryOp::~AstUnaryOp()
{
    delete m_operand;
}

std::string AstUnaryOp::ToString(Op op)
{
    switch(op)
    {
    case MINUS:
	return "-";
    case PLUS:
	return "+";
    case NOT:
	return " not ";
    default:
	assert ( 0 );
	return "";
    }
}

SteelType *AstUnaryOp::lvalue(SteelInterpreter *pInterpreter)
{
    return NULL;
}

SteelType AstUnaryOp::evaluate(SteelInterpreter *pInterpreter)
{
    try{
    
	switch(m_op)
	{
	case MINUS:
	    return - m_operand->evaluate(pInterpreter);
	case PLUS:
	    return m_operand->evaluate(pInterpreter);
	case NOT:
	    return ! m_operand->evaluate(pInterpreter);
	}
    }
    catch(OperationMismatch m)
    {
	throw SteelException(SteelException::RUNTIME,
			     GetLine(),GetScript(),
			     "Unary operation '" + ToString(m_op) + "' disallowed on this type");
    }

	assert ( 0 );
	return SteelType();
}

ostream & AstUnaryOp::print(std::ostream &out)
{
    out << ToString(m_op) << *m_operand;

    return out;
}

AstCallExpression::AstCallExpression(unsigned int line,
				     const std::string &script, AstFuncIdentifier *pId, AstParamList *pList)
    :AstExpression(line,script),m_pId(pId),m_pParams(pList)
{
    assert ( m_pId );
}
AstCallExpression::~AstCallExpression()
{
    delete m_pId;
    delete m_pParams;
}

SteelType AstCallExpression::evaluate(SteelInterpreter *pInterpreter)
{
    SteelType ret;
    try{
	if(m_pParams)
	    ret = pInterpreter->call( m_pId->getValue(), m_pParams->getParamList(pInterpreter) );
	else ret = pInterpreter->call( m_pId->getValue(), std::vector<SteelType>() );
    }
    catch(ParamMismatch )
    {
	throw SteelException(SteelException::PARAM_MISMATCH,
			     GetLine(),GetScript(),
			     "Function '" + m_pId->getValue() + "' called with incorrect number of parameters.");
    }
    catch(UnknownIdentifier )
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(), GetScript(),
			     "Unknown function: '" + m_pId->getValue() + '\'');
    }


    return ret;
}

ostream & AstCallExpression::print(std::ostream &out)
{
    if(m_pParams)
    {
	out << *m_pId 
	    << '(' << *m_pParams << ')';
    }
    else
    {
	out << *m_pId << "()";
    }
    
    return out;
}
/*
AstArrayExpression::AstArrayExpression(unsigned int line,
				       const std::string &script,
				       AstArrayIdentifier *pId,
				       AstExpression * pExp)
    :AstExpression(line,script),m_pId(pId),m_pExpression(pExp)
{
}

AstArrayExpression::~AstArrayExpression()
{
    delete m_pId;
    delete m_pExpression;
}

ostream & AstArrayExpression::print(std::ostream &out)
{
    out << *m_pId << '[' << *m_pExpression 
	<< ']';
    return out;
}
*/

AstArrayElement::AstArrayElement(unsigned int line,
				 const std::string &script,
				 AstExpression *pLValue,
				 AstExpression *pExp)
    :AstExpression(line,script),m_pLValue(pLValue),m_pExp(pExp)
{
    
}

AstArrayElement::~AstArrayElement()
{
    delete m_pLValue;
    delete m_pExp;
}

int AstArrayElement::getArrayIndex(SteelInterpreter *pInterpreter) const
{
    return m_pExp->evaluate(pInterpreter);
}


SteelType * AstArrayElement::lvalue(SteelInterpreter *pInterpreter)
{
    try
    {
	
	SteelType * pArray = m_pLValue->lvalue(pInterpreter);
	if(NULL == pArray)
	{
	    throw SteelException(SteelException::INVALID_LVALUE,
				 GetLine(),
				 GetScript(),
				 "Invalid lvalue before subscript.");
	}

	if(!pArray->isArray())
	{
	    throw SteelException(SteelException::TYPE_MISMATCH,
				 GetLine(),
				 GetScript(),
				 "Lvalue is not an array.");
	}

	int index = m_pExp->evaluate(pInterpreter);
	
	// It will throw out of bounds, or what not.
	return pArray->getLValue(index);
    }
    catch(UnknownIdentifier)
    {
	// TODO: Make these exceptions carry a god damn string.
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier in array lvalue.");
			     
    }
    catch(OutOfBounds)
    {
	throw SteelException(SteelException::OUT_OF_BOUNDS,
			     GetLine(),
			     GetScript(),
			     "lvalue subscript was out of bounds.");
    }

    return NULL;
}

SteelType AstArrayElement::evaluate(SteelInterpreter *pInterpreter)
{
    try
    {
	SteelType * pL = m_pLValue->lvalue(pInterpreter);
	if(NULL == pL)
	    throw SteelException(SteelException::INVALID_LVALUE,
				 GetLine(),
				 GetScript(),
				 "Invalid lvalue in array expression.");
	
	return pInterpreter->lookup(pL, m_pExp->evaluate(pInterpreter));
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier.");
    }
    catch(OutOfBounds)
    {
	throw SteelException(SteelException::OUT_OF_BOUNDS,
			     GetLine(),
			     GetScript(),
			     "Array index out of bounds.");
    }

}

ostream & AstArrayElement::print(std::ostream &out)
{
    out << *m_pLValue << '[' << *m_pExp << ']' ;
	return out;
}

SteelType * AstArrayIdentifier::lvalue(SteelInterpreter *pInterpreter)
{
    try
    {
	return pInterpreter->lookup_lvalue(getValue());
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown array identifier:'" + getValue() + '\'');

    }

    return NULL;
}


SteelType AstArrayIdentifier::evaluate(SteelInterpreter *pInterpreter)
{
    
    // Find our reference variable in the file. 
    SteelType var;

    try {
	var = pInterpreter->lookup(getValue()); 
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown array identifier:'" + getValue() + '\'');
    }

    return var;
}


AstVarAssignmentExpression::AstVarAssignmentExpression(unsigned int line,
						       const std::string &script,
						       AstExpression *pLValue,
						       AstExpression *pExp)
    :AstExpression(line,script),m_pLValue(pLValue),m_pExpression(pExp)
{

}

AstVarAssignmentExpression::~AstVarAssignmentExpression()
{
    delete m_pLValue;
    delete m_pExpression;
}

SteelType *AstVarAssignmentExpression::lvalue(SteelInterpreter *pInterpreter)
{
    // TODO : Is this an lvalue or not??

    

    return NULL;
}

SteelType AstVarAssignmentExpression::evaluate(SteelInterpreter *pInterpreter)
{
    SteelType exp = m_pExpression->evaluate(pInterpreter);
    try
    {
	SteelType *pL = m_pLValue->lvalue(pInterpreter);
	if(pL == NULL)
	    throw SteelException(SteelException::INVALID_LVALUE,
				 GetLine(),
				 GetScript(),
				 "Left of '=' is not a valid lvalue.");

	pInterpreter->assign ( pL, exp );
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier");
    }
    catch(TypeMismatch)
    {
	throw SteelException(SteelException::TYPE_MISMATCH,
			     GetLine(),
			     GetScript(),
			     "Illegal assignment due to type mismatch.");
    }

    return exp;
}

ostream & AstVarAssignmentExpression::print(std::ostream &out)
{
    out << *m_pLValue << '=' << *m_pExpression;
    return out;
}



AstParamList::AstParamList(unsigned int line,
			   const std::string &script)
    :AstBase(line,script)
{
}
AstParamList::~AstParamList()
{
    for(std::list<AstExpression*>::iterator i = m_params.begin();
	i != m_params.end(); i++) delete *i;
}

std::vector<SteelType> AstParamList::getParamList(SteelInterpreter *pInterpreter) const 
{
    std::vector<SteelType> params;

    for(std::list<AstExpression*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
    {
	
	SteelType var = (*i)->evaluate(pInterpreter);
	params.push_back ( var );
    }
    

    return params;
}

void AstParamList::add(AstExpression *pExp)
{
    m_params.push_back(pExp);
}
ostream & AstParamList::print(std::ostream &out)
{
    for(std::list<AstExpression*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
    {
	std::list<AstExpression*>::const_iterator next = i ;
	next++;

	if(next == m_params.end())
	    out << *(*i);
	else
	    out  << *(*i) << ',';
    }
    

    return out;
}

SteelType * AstVarIdentifier::lvalue(SteelInterpreter *pInterpreter)
{
    try
    {
	return pInterpreter->lookup_lvalue ( getValue() );
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier:'" + getValue() + '\'');
    }

    return NULL;
};

SteelType AstVarIdentifier::evaluate(SteelInterpreter *pInterpreter)
{
    try
    {
	SteelType val = pInterpreter->lookup ( getValue() );
	return val;
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier:'" + getValue() + '\'');
    }
}

void AstDeclaration::setValue(const SteelType &value)
{
    m_bHasValue = true;
    m_value = value;
}


AstVarDeclaration::AstVarDeclaration(unsigned int line,
				     const std::string &script,
				     AstVarIdentifier *pId,
				     AstExpression *pExp)
    :AstDeclaration(line,script),m_pId(pId),m_pExp(pExp)
{
}

AstVarDeclaration::~AstVarDeclaration()
{
    delete m_pId;
    delete m_pExp;
}

AstStatement::eStopType AstVarDeclaration::execute(SteelInterpreter *pInterpreter)
{
    try{
	pInterpreter->declare(m_pId->getValue());

	SteelType * pVar = pInterpreter->lookup_lvalue( m_pId->getValue() );
	// If this is null, its crazy, because we JUST declared its ass.
	assert ( NULL != pVar);
	
	if(m_pExp) 
	    pInterpreter->assign( pVar, m_pExp->evaluate(pInterpreter) );

	// If there is a value, it overrides whatever the expression was.
	// This is for parameters
	if(m_bHasValue) pInterpreter->assign( pVar, m_value);
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier: '" + m_pId->getValue() + '\'');
    }

    return COMPLETED;
}

ostream & AstVarDeclaration::print(std::ostream &out)
{
    out << "var " << *m_pId;

    if( m_pExp ) out << '=' << *m_pExp << ';' << std::endl;
    return out;
}


AstArrayDeclaration::AstArrayDeclaration(unsigned int line,
					 const std::string &script,
					 AstArrayIdentifier *pId,
					 AstExpression *pInt)
    :AstDeclaration(line,script),m_pId(pId),m_pIndex(pInt),m_pExp(NULL)
{
}

AstArrayDeclaration::~AstArrayDeclaration()
{
    delete m_pId;
    delete m_pIndex;
    delete m_pExp;
}

void AstArrayDeclaration::assign(AstExpression *pExp)
{
    // Todo: Actually evaluate this here and toss it
    m_pExp = pExp;
}


AstStatement::eStopType AstArrayDeclaration::execute(SteelInterpreter *pInterpreter)
{
    if(m_pIndex)
	pInterpreter->declare_array( m_pId->getValue(), m_pIndex->evaluate(pInterpreter));
    else pInterpreter->declare_array( m_pId->getValue(), 0);

    try{
	SteelType * pVar = pInterpreter->lookup_lvalue( m_pId->getValue() );
	// If this is null here, we're in a BAD way. Programming error.
	assert ( NULL != pVar);
	if(m_pExp)
	    pInterpreter->assign( pVar, m_pExp->evaluate(pInterpreter) );

	if(m_bHasValue) pInterpreter->assign( pVar, m_value);
    }
    catch(TypeMismatch)
    {
	throw SteelException(SteelException::TYPE_MISMATCH,
			     GetLine(),
			     GetScript(),
			     "Attempt to assign non-array to array:'" + m_pId->getValue() + '\'');
    }
    catch(UnknownIdentifier)
    {
	throw SteelException(SteelException::UNKNOWN_IDENTIFIER,
			     GetLine(),
			     GetScript(),
			     "Unknown identifier in assignment.");
    }

    return COMPLETED;
}

ostream & AstArrayDeclaration::print(std::ostream &out)
{
    out << "var " << *m_pId ;
    if(m_pIndex) out << '[' << *m_pIndex << ']';
    if(m_pExp) out << '=' << *m_pExp;
    out << ';' << std::endl;
    return out;
}



AstParamDefinitionList::AstParamDefinitionList(unsigned int line,
					       const std::string &script)
    :AstBase(line,script)
{
}

AstParamDefinitionList::~AstParamDefinitionList()
{
    for(std::list<AstDeclaration*>::iterator i = m_params.begin();
	    i != m_params.end(); i++) 
	delete *i;
}

void AstParamDefinitionList::add(AstDeclaration *pDef)
{
    m_params.push_back( pDef );
}

void AstParamDefinitionList::executeDeclarations(SteelInterpreter *pInterpreter)
{
    for(std::list<AstDeclaration*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
	(*i)->execute(pInterpreter);
}

int AstParamDefinitionList::size() const
{
    return m_params.size();
}


void AstParamDefinitionList::executeDeclarations(SteelInterpreter *pInterpreter, 
						 const std::vector<SteelType> &params)
{
    assert ( params.size() == m_params.size() );


    int parameter = 0;
    for(std::list<AstDeclaration*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
    {
	(*i)->setValue(params[parameter++]); 
	(*i)->execute(pInterpreter);
    }
}


ostream & AstParamDefinitionList::print (std::ostream &out)
{
    for(std::list<AstDeclaration*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
    {
	std::list<AstDeclaration*>::const_iterator next = i ;
	next++;

	if(next == m_params.end())
	    out << *(*i);
	else
	    out  << *(*i) << ',';
    }
    return out;
}	       


AstFunctionDefinition::AstFunctionDefinition(unsigned int line,
					     const std::string &script,
					     AstFuncIdentifier *pId,
					     AstParamDefinitionList *pParams,
					     AstStatementList * pStmts)
    :AstStatement(line,script),m_pId(pId),m_pParams(pParams),m_pStatements(pStmts)
{
    
}
AstFunctionDefinition::~AstFunctionDefinition()
{
    delete m_pId;
    delete m_pParams;
    delete m_pStatements;
}

AstStatement::eStopType AstFunctionDefinition::execute(SteelInterpreter *pInterpreter)
{
    try{
	pInterpreter->registerFunction( m_pId->getValue(), m_pParams , m_pStatements );
    }
    catch(AlreadyDefined)
    {
	throw SteelException(SteelException::FUNCTION_DEFINED,
			     GetLine(),
			     GetScript(),
			     "Function '" + m_pId->getValue() + "'already defined!");
    }

    return COMPLETED;
}

ostream & AstFunctionDefinition::print (std::ostream &out)
{
    out << "function " << *m_pId  << '(' ;
    if(m_pParams) out << *m_pParams;
    out << ')' << *m_pStatements << std::endl;
    return out;
}


ostream & operator<<(ostream & out,AstBase & ast)
{
    return ast.print(out);
}


