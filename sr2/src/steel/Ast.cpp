#include "Ast.h"
#include <string>
#include <iostream>
#include <cassert>

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

void AstInteger::print()
{
    std::cout << m_value;
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

void AstString::print()
{
    std::cout << m_value;
}


AstFloat::AstFloat(unsigned int line,
		   const std::string &script,
		   double value)
    : AstExpression(line,script),m_value(value)
{
    
}

void AstFloat::print()
{
    std::cout << m_value;
}

AstIdentifier::AstIdentifier(unsigned int line,
			     const std::string &script,
			     const std::string &value)
    : AstExpression(line,script),m_value(value)
{
    
}

void AstIdentifier::print()
{
    std::cout << m_value;
}



AstStatement::AstStatement(unsigned int line, const std::string &script)
    :AstBase(line,script)
{
}

AstStatement::~AstStatement()
{
}

void AstStatement::print()
{
    std::cout << "Empty Statement";
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

void AstExpressionStatement::print()
{
    std::cout << "STATEMENT (\t";
    m_pExp->print();
    std::cout << ")";
}

AstScript::AstScript(unsigned int line, const std::string &script)
    :AstBase(line,script),m_pList(NULL)
{
}
AstScript::~AstScript()
{
    delete m_pList;
}

void AstScript::print()
{
    std::cout << "SCRIPT[\t";
    if(m_pList) m_pList->print();
    std::cout << "]";
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

void AstStatementList::print()
{
    std::cout << "STMT LIST {";
    for(std::list<AstStatement*>::const_iterator it = m_list.begin();
	it != m_list.end(); it++)
    {
	std::cout << "\t";
	AstStatement * pStatement = *it;
	pStatement->print();
    }
    std::cout << "}";
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

void AstWhileStatement::print()
{
    std::cout << "WHILE (";
    m_pCondition->print();
    std::cout << ")\n\t";
    m_pStatement->print();
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

void AstIfStatement::print()
{
    std::cout << "IF (";
    m_pCondition->print();
    std::cout << ')';
    std::cout << '\t';
	m_pStatement->print();

    if(m_pElse) 
    {
	std::cout << " ELSE ";
	m_pElse->print();
    }
}

AstReturnStatement::AstReturnStatement(unsigned int line, const std::string &script, AstExpression *pExp)
    :AstStatement(line,script),m_pExpression(pExp)
{
}

AstReturnStatement::~AstReturnStatement()
{
    delete m_pExpression;
}

void AstReturnStatement::print()
{
    std::cout << "RETURN ";
    if(m_pExpression)
    {
	m_pExpression->print();
    }
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

void AstLoopStatement::print()
{
    std::cout << "LOOP (";
    m_pCountExpression->print();
    std::cout << ")\t";
    m_pStatement->print();
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

void AstBinOp::print()
{
    std::cout << "OP";
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

void AstUnaryOp::print()
{
    std::cout << "UNOP";
}

AstCallExpression::AstCallExpression(unsigned int line,
				     const std::string &script, AstFuncIdentifier *pId, AstParamList *pList)
    :AstExpression(line,script),m_pId(pId),m_pParams(pList)
{
}
AstCallExpression::~AstCallExpression()
{
    delete m_pId;
    delete m_pParams;
}

void AstCallExpression::print()
{
    std::cout << "CALL: ";
    m_pId->print();
    std::cout << '(';
    m_pParams->print();
    std::cout << ')';
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

void AstArrayExpression::print()
{
    std::cout << '\t';
    m_pId->print();
    m_pExpression->print();
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

void AstVarAssignmentExpression::print()
{
    m_pId->print();
    std::cout << '=' ;
    m_pExpression->print();
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
void AstParamList::print()
{
    for(std::list<AstExpression*>::const_iterator i = m_params.begin();
	i != m_params.end(); i++)
    {
	std::cout << '\t';
	(*i)->print();
    }
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
void AstVarDeclaration::print()
{
    
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
void AstArrayDeclaration::print()
{
}
