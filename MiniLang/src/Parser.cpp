/*
 * Parser.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: sandro
 */

#include "Parser.h"

Parser::Parser(Lexer p_lexer) : Lex(p_lexer) {
	// TODO Auto-generated constructor stub
	auto ASTTree = Parse();
	ASTTree->PrintInfo(0);
}

Parser::~Parser() {
	// TODO Auto-generated destructor stub
}

std::unique_ptr<ASTExprNode> Parser::Error(const char *p_Str)
{
	std::cout << "Error: " << p_Str << std::endl;
	return nullptr;
}

std::unique_ptr<ASTExprNode> Parser::ParseNumberExpr()
{
	auto node = make_unique<ASTNumberExprNode>(CurrentToken.number_value);
	CurrentToken = Lex.GetToken();
	return std::move(node);
}

std::unique_ptr<ASTExprNode> Parser::ParseParenthesisExpr()
{
	CurrentToken = Lex.GetToken();
	auto node = ParseExpression();
	if (!node) return nullptr;
	if (CurrentToken.token_type != Lexer::TOK_PUNC)
	{
		std::string err_msg = "Encountered " + CurrentToken.ToString() + ", expecting TOK_PUNC [)]";
		return Error(err_msg.c_str());
	}
	else
	{
		CurrentToken = Lex.GetToken();
		return node;
	}
}

std::unique_ptr<ASTExprNode> Parser::ParseIdentifierExpr()
{
	std::string Name = CurrentToken.id_name;
	CurrentToken = Lex.GetToken();
	if (CurrentToken.token_type != Lexer::TOK_PUNC)
	{
		return make_unique<ASTVariableExprNode>(Name);
	}
	else
	{
		if (CurrentToken.id_name.compare("(") == 0)
		{
			CurrentToken = Lex.GetToken();
			//This is a function call. Start argument parsing.
			std::vector<std::unique_ptr<ASTExprNode>> args;
			while (true && CurrentToken.token_type!=Lexer::TOK_EOF)
			{
				if (auto arg = ParseExpression())
					args.push_back(std::move(arg));
				else
				{
					std::string err_msg = "Couldn't parse argument list for function " + Name;
					return Error(err_msg.c_str());
				}

				if (CurrentToken.token_type == Lexer::TOK_PUNC)
				{
					if (CurrentToken.id_name.compare(",") == 0)
					{
						//Parse the next argument - do not break the loop
						CurrentToken = Lex.GetToken();
						continue;
					}

					if (CurrentToken.id_name.compare(")") == 0)
					{
						//We're done. Break the loop
						break;
					}

					std::string err_msg = "Encountered " + CurrentToken.ToString() + ", expecting TOK_PUNC [)] or [,]";
					return Error(err_msg.c_str());
				}
			}

			CurrentToken = Lex.GetToken();
			return make_unique<ASTCallExprNode>(Name, std::move(args));
		}

		if (CurrentToken.id_name.compare("[") == 0)
		{
			//Identifier is an array
		}
	}

	return nullptr;
}

std::unique_ptr<ASTExprNode> Parser::ParseUnaryExpr()
{
	switch (CurrentToken.token_type)
	{
		case Lexer::TOK_ID:
			return ParseIdentifierExpr();
		case Lexer::TOK_NUMBER:
			return ParseNumberExpr();
		case Lexer::TOK_PUNC:
			if (CurrentToken.id_name.compare("(") == 0)
				return ParseParenthesisExpr();
			break;
		default: return Error("Oops, got mixed UP!!");
	}
	return nullptr;
}

std::unique_ptr<ASTExprNode> Parser::ParseBinaryExpr(int p_Precedence, std::unique_ptr<ASTExprNode> p_LHS)
{
	while (true)
	{
		if (CurrentToken.token_type == Lexer::TOK_ARITHMETICOP)
		{
			float op_prec = CurrentToken.number_value;
			std::string op_sym = CurrentToken.id_name;

			CurrentToken = Lex.GetToken();

			auto RHS = ParseUnaryExpr();
			if (!RHS)
				return nullptr;

			float nxt_op_prec = CurrentToken.number_value;

			if (op_prec < nxt_op_prec)
			{
				RHS = ParseBinaryExpr(op_prec+1, std::move(RHS));
				if (!RHS)
					return nullptr;
			}

			p_LHS = make_unique<ASTBinaryExprNode>(op_sym[0],std::move(p_LHS), std::move(RHS));
		}
		else
		{
			//next token is not an arithmetic op, just return this node.
			return p_LHS;
		}

		float op_prec = CurrentToken.number_value;
		std::string op_sym = CurrentToken.id_name;
	}
	return nullptr;
}

std::unique_ptr<ASTExprNode> Parser::ParseExpression()
{
	auto LHS = ParseUnaryExpr();
	if (!LHS)
		return nullptr;

	return ParseBinaryExpr(0, std::move(LHS));
}

std::unique_ptr<ASTStatementNode> Parser::ParseIfStatement()
{
	auto node = make_unique<ASTIfStatementNode>();
	CurrentToken = Lex.GetToken();
	//return std::move(node);
	return nullptr;
}

std::unique_ptr<ASTStatementNode> Parser::ParseAssignmentStatement()
{
	std::string var_name = CurrentToken.id_name;
	CurrentToken = Lex.GetToken();
	if (CurrentToken.token_type != Lexer::TOK_ASSIGNOP)
	{
		Error("Expecting ':=' while parsing an assignment statement");
		return nullptr;
	}
	CurrentToken = Lex.GetToken();
	auto expr_node = ParseExpression();
	auto ass_node = make_unique<ASTAssignmentStatementNode>(var_name.c_str(), std::move(expr_node));

	//return std::move(ass_node);
	//return nullptr;
}

std::unique_ptr<ASTFuncPrototypeNode> Parser::ParseFunctionPrototype() {
	CurrentToken = Lex.GetToken();
	if (CurrentToken.token_type != Lexer::TOK_ID) {
		Error("Expecting function name");
		return nullptr;
	}

	std::string functionName = CurrentToken.id_name;
	auto functionParameters = std::vector<std::string>();

	CurrentToken = Lex.GetToken();
	if (CurrentToken.token_type != Lexer::TOK_PUNC) {
		if (CurrentToken.id_name.compare("(") == 0)
		{
			CurrentToken = Lex.GetToken();
			while (CurrentToken.token_type != Lexer::TOK_PUNC)
			{
				if (CurrentToken.token_type == Lexer::TOK_ID)
				{
					functionParameters.push_back(CurrentToken.id_name);
					CurrentToken = Lex.GetToken();
					if (CurrentToken.token_type == Lexer::TOK_PUNC)
					{
						if (CurrentToken.id_name.compare(",") == 0)
						{
							CurrentToken = Lex.GetToken();
						}
						else
						{
							std::string errorMsg = "Expecting , between parameter variables of function ";
							errorMsg.append(functionName);
							Error(errorMsg.c_str());
							return nullptr;
						}
					}
				}
				else
				{
					std::string errorMsg = "Expecting function parameter variables after function ";
					errorMsg.append(functionName);
					Error(errorMsg.c_str());
					return nullptr;
				}
			}
			if (CurrentToken.id_name.compare(")"))
			{
				//Create and return a function prototype AST node
				return make_unique<ASTFuncPrototypeNode>(functionName.c_str(), functionParameters);
			}
		}
	}

	std::string errorMsg = "Expecting ( after function name ";
	errorMsg.append(functionName);
	Error(errorMsg.c_str());
	return nullptr;
}

std::unique_ptr<ASTStatementNode> Parser::ParseFunctionBody()
{
	//std::unique_ptr<ASTStatementNode> statement_node;
	auto statement_node = ParseAssignmentStatement();
	return statement_node;
}

std::unique_ptr<ASTFunctionNode> Parser::ParseFunctionDefinition()
{
	CurrentToken = Lex.GetToken();
	auto prototype_node = ParseFunctionPrototype();
	if (!prototype_node)
		return nullptr;

	auto functionBody_node = ParseFunctionBody();

	return make_unique<ASTFunctionNode>(std::move(prototype_node), std::move(functionBody_node));
}

std::unique_ptr<ASTExprNode> Parser::Parse()
{
	std::cout << "[Parser] Start" << std::endl;
	CurrentToken = Lex.GetToken();
	while (CurrentToken.token_type != Lexer::TOK_EOF)
	{
		//std::cout << CurrentToken.ToString() << " ";
		//CurrentToken = Lex.GetToken();
		switch(CurrentToken.token_type)
		{
		case Lexer::TOK_DEF:
//			auto p = ParseFunctionDefinition();
//				p->PrintInfo(0);
			break;
		default:
			auto pe = ParseExpression();
				pe->PrintInfo(0);
			break;

		}
	}
	std::cout << "\n[Parser] Finish" << std::endl;
	return nullptr;
}
