#include "Ast.h"
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

void AstScript::SetList(AstStatementList *pList)
{
    m_pList = pList;
}

void AstScript::SetFunctionList( )
{
}

AstStatementList::AstStatementList(unsigned int line, const std::string &script)
    :AstStatement(line,script)
{
}

AstStatementList::~AstStatementList()
{
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
				   AstExpression *pExp, AstVarIdentifier *pId, AstStatement * pStmt)
    :AstStatement(line,script),m_pCountExpression(pExp),m_pIterator(pId),m_pStatement(pStmt)
{
}

AstLoopStatement::~AstLoopStatement()
{
    delete m_pCountExpression;
    delete m_pIterator;
    delete m_pStatement;
}

ostream & AstLoopStatement::print(std::ostream &out)
{
    out << "loop (" << *m_pCountExpression << ')' << " times using " << *m_pIterator << "\n" << *m_pStatement;
    
    return out;
}





AstBinOp::AstBinOp(unsigned int line,
		   const std::string &script,
		   Op op, AstExpression *right, AstExpression *left)
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

ostream & AstBinOp::print(std::ostream &out)
{
    out << *m_right <<  ToString(m_op) << *m_left;

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


AstVarAssignmentExpression::AstVarAssignmentExpression(unsigned int line,
						       const std::string &script,
						       AstVarIdentifier *pId,
						       AstExpression *pExp)
    :AstExpression(line,script),m_pId(pId),m_pExpression(pExp)
{

}

AstVarAssignmentExpression::~AstVarAssignmentExpression()
{
    delete m_pId;
    delete m_pExpression;
}

ostream & AstVarAssignmentExpression::print(std::ostream &out)
{
    out << *m_pId << '=' << *m_pExpression;
    return out;
}

AstParamList::AstParamList(unsigned int line,
			   const std::string &script)
    :AstBase(line,script)
{
}
AstParamList::~AstParamList()
{
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
ostream & AstVarDeclaration::print(std::ostream &out)
{
    out << "var " << *m_pId;

    if( m_pExp ) out << '=' << *m_pExp << ';' << std::endl;
    return out;
}

AstArrayDeclaration::AstArrayDeclaration(unsigned int line,
					 const std::string &script,
					 AstArrayIdentifier *pId,
					 AstInteger *pInt)
    :AstDeclaration(line,script),m_pId(pId),m_pIndex(pInt)
{
}

AstArrayDeclaration::~AstArrayDeclaration()
{
    delete m_pId;
    delete m_pIndex;
}
ostream & AstArrayDeclaration::print(std::ostream &out)
{
    out << *m_pId << '[' << *m_pIndex << ']';
    return out;
}



ostream & operator<<(ostream & out,AstBase & ast)
{
    return ast.print(out);
}
