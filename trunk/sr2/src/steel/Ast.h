#ifndef STEEL_AST_H
#define STEEL_AST_H

#include <string>
#include <list>
#include <iostream>

class AstBase
{
public:
    AstBase(unsigned int line, const std::string &script);
    virtual ~AstBase();

    unsigned int GetLine() const { return  m_line; }
    const std::string GetScript() const { return m_script_name; }
    virtual void print()=0;
private:
    unsigned int m_line;
    std::string m_script_name;
};

class AstKeyword : public AstBase
{
public:
    AstKeyword(unsigned int line, const std::string &script):AstBase(line,script){}
    virtual ~AstKeyword(){}

    virtual void print(){}
private:
};

class AstExpression;

class AstStatement : public AstBase
{
public:
    AstStatement(unsigned int line, const std::string &script);
    virtual ~AstStatement();

    virtual void print();
private:
};


class AstExpressionStatement : public AstStatement
{
public:
    AstExpressionStatement(unsigned int line, const std::string &script, AstExpression *pExp);
    virtual ~AstExpressionStatement();

    virtual void print();
private:
    AstExpression *m_pExp;
};

class AstStatementList;

class AstScript : public AstBase
{
public:
    AstScript(unsigned int line, const std::string &script);
    virtual ~AstScript();

    virtual void print();
    void SetList( AstStatementList * pStatement);
    void SetFunctionList( );
private:
    AstStatementList *m_pList;
};


class AstStatementList : public AstStatement
{
public:
    AstStatementList(unsigned int line, const std::string &script);
    virtual ~AstStatementList();

    virtual void print();
    void add(AstStatement *pStatement) { m_list.push_back(pStatement); }
private:
    std::list<AstStatement*> m_list;
};

class AstWhileStatement : public AstStatement
{
public:
    AstWhileStatement(unsigned int line, const std::string &script, AstExpression *pExp, AstStatement *pStmt);
    virtual ~AstWhileStatement();

    virtual void print();
    
	
private:
    AstExpression * m_pCondition;
    AstStatement * m_pStatement;
};

class AstIfStatement : public AstStatement
{
public:
    AstIfStatement(unsigned int line, const std::string &script,
		   AstExpression *pExp, AstStatement *pStmt,
		   AstStatement *pElse=NULL);
    virtual ~AstIfStatement();

    virtual void print();
private:
    AstExpression *m_pCondition;
    AstStatement *m_pElse;
    AstStatement * m_pStatement;
};

class AstReturnStatement : public AstStatement
{
public:
    AstReturnStatement(unsigned int line, const std::string &script, AstExpression *pExp = NULL);
    virtual ~AstReturnStatement();

    virtual void print();
private:
    AstExpression *m_pExpression;
};


class AstBreakStatement : public AstStatement
{
public:
    AstBreakStatement(unsigned int line, const std::string &script)
	:AstStatement(line,script){}
	virtual ~AstBreakStatement(){}

    virtual void print(){ std::cout << "BREAK" ;}
private:

};

class AstContinueStatement : public AstStatement
{
public:
    AstContinueStatement(unsigned int line, const std::string &script)
	:AstStatement(line,script){}
	virtual ~AstContinueStatement(){}

	virtual void print(){ std::cout << "CONTINUE"; }
private:

};

class AstVarIdentifier;

class AstLoopStatement : public AstStatement
{
public:
    AstLoopStatement(unsigned int line, const std::string &script,
		     AstExpression *pExp, AstVarIdentifier *pId, AstStatement * pStmt);
    
    virtual ~AstLoopStatement();

    virtual void print();
private:
    AstExpression *m_pCountExpression;
    AstVarIdentifier *m_pIterator;
    AstStatement * m_pStatement;
};


class AstExpression : public AstBase
{
public:
    AstExpression(unsigned int line,
		  const std::string &script)
	:AstBase(line,script){}
	virtual ~AstExpression(){}
private:
};


class AstInteger : public AstExpression
{
public:
    AstInteger(unsigned int line, const std::string &script, int value);
    virtual ~AstInteger(){}

    virtual void print();
private:
    int m_value;
};

class AstString : public AstExpression
{
public:
    AstString(unsigned int line,
	      const std::string &script);
    virtual ~AstString(){}

    virtual void print();
    void addChar(const char c);
    void addString(const std::string &str);
private:
    std::string m_value;
};

class AstFloat : public AstExpression
{
public:
    AstFloat(unsigned int line,
	     const std::string &script,
	     double value);
    virtual ~AstFloat(){}

    virtual void print();
private:
    double m_value;
};

class AstIdentifier : public AstExpression
{
public:
    AstIdentifier(unsigned int line,
		     const std::string &script,
		     const std::string &value);
    virtual ~AstIdentifier(){}
    virtual void print();
private:
    std::string m_value;
};

class AstVarIdentifier : public AstIdentifier
{
public:
    AstVarIdentifier(unsigned int line,
		     const std::string &script,
		     const std::string &value)
	:AstIdentifier(line,script,value){}
    virtual ~AstVarIdentifier(){}

private:
};

class AstFuncIdentifier : public AstIdentifier
{
public:
    AstFuncIdentifier(unsigned int line,
		      const std::string &script,
		      const std::string &value)
	:AstIdentifier(line,script,value){}
    virtual ~AstFuncIdentifier(){}

private:
};

class AstArrayIdentifier : public AstIdentifier
{
public:
    AstArrayIdentifier(unsigned int line,
		       const std::string &script,
		       const std::string &value)
	:AstIdentifier(line,script,value){}
    virtual ~AstArrayIdentifier(){}
private:
};


class AstBinOp : public AstExpression
{
public:
    enum Op
    {
	ADD,
	SUB,
	MULT,
	DIV,
	MOD,
	AND,
	OR,
	D,
	POW,
	EQ,
	NE,
	LT,
	GT,
	LTE,
	GTE
    };

    AstBinOp(unsigned int line,
	     const std::string &script,
	     Op op, AstExpression *right, AstExpression *left);
    virtual ~AstBinOp();

    virtual void print();
	     
private:
    Op m_op;
    AstExpression *m_right;
    AstExpression *m_left;
};

class AstUnaryOp : public AstExpression
{
public:
    enum Op
    {
	MINUS,
	PLUS,
	NOT
    };
    AstUnaryOp(unsigned int line,
	       const std::string &script, Op op,
	       AstExpression *operand);

    virtual ~AstUnaryOp();
    virtual void print();
private:
    Op m_op;
    AstExpression *m_operand;
};


class AstParamList;

class AstCallExpression : public AstExpression
{
public:
    AstCallExpression(unsigned int line,
		      const std::string &script, AstFuncIdentifier *pId, AstParamList *pList=NULL);
    virtual ~AstCallExpression();

    virtual void print();
private:
    AstFuncIdentifier *m_pId;
    AstParamList *m_pParams;
};

class AstArrayExpression : public AstExpression
{
public:
    AstArrayExpression(unsigned int line,
		       const std::string &script,
		       AstArrayIdentifier * pId,
		       AstExpression * pExp);
    virtual ~AstArrayExpression();

    virtual void print();
private:
    AstArrayIdentifier *m_pId;
    AstExpression * m_pExpression;
};

class AstVarAssignmentExpression : public AstExpression
{
public:
    AstVarAssignmentExpression(unsigned int line,
			    const std::string &script,
			    AstVarIdentifier *pId,
			    AstExpression *pExp);

    virtual ~AstVarAssignmentExpression();

    virtual void print();
private:
    AstVarIdentifier * m_pId;
    AstExpression * m_pExpression;
};

class AstParamList : public AstBase
{
public:
    AstParamList(unsigned int line,
		 const std::string &script);
    virtual ~AstParamList();

    void add(AstExpression *pExp);
    virtual void print();
private:
    std::list<AstExpression*> m_params;
};

class AstDeclaration : public AstStatement
{
public:
    AstDeclaration(unsigned int line,
			const std::string &script)
	:AstStatement(line,script){}
	virtual ~AstDeclaration(){}
private:
};

class AstVarDeclaration : public AstDeclaration
{
public:
    AstVarDeclaration(unsigned int line,
		      const std::string &script,
		      AstVarIdentifier *pId,
		      AstExpression *pExp = NULL);
    virtual ~AstVarDeclaration();
    virtual void print();
private:
    AstVarIdentifier *m_pId ;
    AstExpression * m_pExp;
};

class AstArrayDeclaration: public AstDeclaration
{
public:
    AstArrayDeclaration(unsigned int line,
			const std::string &script,
			AstArrayIdentifier *pId,
			AstInteger *pInt);
    virtual ~AstArrayDeclaration();
    virtual void print();
private:
    AstArrayIdentifier *m_pId;
    AstInteger *m_pIndex;
};


#endif
