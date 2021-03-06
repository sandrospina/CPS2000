/*
 * Parser.h
 *
 *  Created on: Jan 6, 2016
 *      Author: sandro
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <vector>

#include "Lexer.h"
//AST Expression Nodes
#include "ASTExprNode.h"
#include "ASTNumberExprNode.h"
#include "ASTVariableExprNode.h"
#include "ASTCallExprNode.h"
#include "ASTBinaryExprNode.h"

//AST Statement Nodes
#include "ASTStatementNode.h"
#include "ASTIfStatementNode.h"
#include "ASTAssignmentStatementNode.h"
#include "ASTDeclarationStatementNode.h"
#include "ASTReturnStatementNode.h"

//AST Function Nodes
#include "ASTFunctionNode.h"
#include "ASTFuncPrototypeNode.h"

#include "ASTProgramNode.h"

//LLVM includes
#include "llvm/IR/Module.h"

class Parser {
public:
	Parser(Lexer p_Lexer);
	virtual ~Parser();

	ASTNode * Parse();

private:
	Lexer Lex;
	Lexer::Token CurrentToken;

	ASTExprNode * Error(const char *Str);
	ASTExprNode * ParseNumberExpr();
	ASTExprNode * ParseParenthesisExpr();
	ASTExprNode * ParseIdentifierExpr();
	ASTExprNode * ParseUnaryExpr();
	ASTExprNode * ParseBinaryExpr(int p_Precedence, ASTExprNode * p_LHS);
	ASTExprNode * ParseExpression();

	ASTFuncPrototypeNode * ParseFunctionPrototype();
	std::vector<ASTStatementNode *> * ParseFunctionBody();
	ASTFunctionNode * ParseFunctionDefinition();

	ASTStatementNode * ParseIfStatement();
	ASTStatementNode * ParseAssignmentStatement();
	ASTStatementNode * ParseReturnStatement();
	ASTStatementNode * ParseDeclarationStatement();
	ASTStatementNode * ParseStatement();

};

#endif /* PARSER_H_ */
