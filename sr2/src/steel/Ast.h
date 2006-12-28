#ifndef STEEL_AST_H
#define STEEL_AST_H

#include <string>
#include <list>
#include <iostream>

using std::ostream;


class AstBase
{
public:
    AstBase(unsigned int line, const std::string &script);
    virtual ~AstBase();

    unsigned int GetLine() const { return  m_line; }
    const std::string GetScript() const { return m_script_name; }
    virtual ostream & print(ostream &out){}
private:
    unsigned int m_line;
    std::string m_script_name;
    friend ostream & operator<<(ostream &,AstBase&);
};

class AstKeyword : public AstBase
{
public:
    AstKeyword(unsigned int line, const std::string &script):AstBase(line,script){}
    virtual ~AstKeyword(){}

    virtual ostream & print(ostream &out){ return out;}
private:
};

class AstExpression;

class AstStatement : public AstBase
{
public:
    AstStatement(unsigned int line, const std::string &script);
    virtual ~AstStatement();

    virtual ostream & print(ostream &out);
private:
};


class AstExpressionStatement : public AstStatement
{
public:
    AstExpressionStatement(unsigned int line, const std::string &script, AstExpression *pExp);
    virtual ~AstExpressionStatement();

    virtual ostream & print(std::ostream &out);
private:
    AstExpression *m_pExp;
};

class AstStatementList;
class AstFunctionDefinitionList;

class AstScript : public AstBase
{
public:
    AstScript(unsigned int line, const std::string &script);
    virtual ~AstScript();

    virtual ostream & print(std::ostream &out);
    void SetList( AstStatementList * pStatement);
    void SetFunctionList( AstFunctionDefinitionList *pList);
private:
    AstFunctionDefinitionList * m_pFunctions;
    AstStatementList *m_pList;
};


class AstStatementList : public AstStatement
{
public:
    AstStatementList(unsigned int line, const std::string &script);
    virtual ~AstStatementList();

    virtual ostream & print(std::ostream &out);
    void add(AstStatement *pStatement) { m_list.push_back(pStatement); }
private:
    std::list<AstStatement*> m_list;
};

class AstWhileStatement : public AstStatement
{
public:
    AstWhileStatement(unsigned int line, const std::string &script, AstExpression *pExp, AstStatement *pStmt);
    virtual ~AstWhileStatement();

    virtual ostream & print(std::ostream &out);
    
	
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

    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
private:
    AstExpression *m_pExpression;
};


class AstBreakStatement : public AstStatement
{
public:
    AstBreakStatement(unsigned int line, const std::string &script)
	:AstStatement(line,script){}
	virtual ~AstBreakStatement(){}

	virtual ostream & print(std::ostream &out){ out << "break;" << std::endl ;}
private:

};

class AstContinueStatement : public AstStatement
{
public:
    AstContinueStatement(unsigned int line, const std::string &script)
	:AstStatement(line,script){}
	virtual ~AstContinueStatement(){}

	virtual ostream & print(std::ostream &out){ out << "continue;" << std::endl; }
private:

};

class AstVarIdentifier;

class AstLoopStatement : public AstStatement
{
public:
    AstLoopStatement(unsigned int line, const std::string &script,
		     AstExpression *pExp, AstVarIdentifier *pId, AstStatement * pStmt);
    
    virtual ~AstLoopStatement();

    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
private:
    int m_value;
};

class AstString : public AstExpression
{
public:
    AstString(unsigned int line,
	      const std::string &script);
    virtual ~AstString(){}

    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
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
    virtual ostream & print(std::ostream &out);
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

    static std::string ToString(Op op);

    AstBinOp(unsigned int line,
	     const std::string &script,
	     Op op, AstExpression *right, AstExpression *left);
    virtual ~AstBinOp();

    virtual ostream & print(std::ostream &out);
	     
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
    static std::string ToString(Op op);
    AstUnaryOp(unsigned int line,
	       const std::string &script, Op op,
	       AstExpression *operand);

    virtual ~AstUnaryOp();
    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
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

    virtual ostream & print(std::ostream &out);
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
    virtual ostream & print(std::ostream &out);
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
    virtual ostream & print(std::ostream &out);
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
    virtual ostream & print(std::ostream &out);
private:
    AstArrayIdentifier *m_pId;
    AstInteger *m_pIndex;
};


class AstParamDefinitionList : public AstBase
{
public:
    AstParamDefinitionList(unsigned int line,
			   const std::string &script);
    virtual ~AstParamDefinitionList();

    void add(AstVarIdentifier *pId);
    virtual ostream & print (std::ostream &out);	       
private:
    std::list<AstVarIdentifier*> m_params;
};

class AstFunctionDefinition : public AstBase
{
public:
    AstFunctionDefinition(unsigned int line,
			  const std::string &script,
			  AstFuncIdentifier *pId,
			  AstParamDefinitionList *pParams,
			  AstStatementList* pStmts);
    virtual ~AstFunctionDefinition();

    virtual ostream & print (std::ostream &out);
private:
    AstFuncIdentifier * m_pId;
    AstParamDefinitionList *m_pParams;
    AstStatementList * m_pStatements;
};

class AstFunctionDefinitionList : public AstBase
{
public:
    AstFunctionDefinitionList(unsigned int line,
			      const std::string &script);

    virtual ~AstFunctionDefinitionList();

    void add(AstFunctionDefinition *pFunc);
    virtual ostream & print (std::ostream &out);
private:
    std::list<AstFunctionDefinition*> m_functions;
};

#endif
