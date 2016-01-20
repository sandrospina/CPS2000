/*
 * ASTVariableExprNode.cpp
 *
 *  Created on: Jan 5, 2016
 *      Author: sandro
 */

#include "ASTVariableExprNode.h"

ASTVariableExprNode::ASTVariableExprNode(const std::string &p_Name) : Name(p_Name) {
	// TODO Auto-generated constructor stub

}

ASTVariableExprNode::~ASTVariableExprNode() {
	// TODO Auto-generated destructor stub
}

void ASTVariableExprNode::PrintInfo(int p_level) {
	std::cout << "ASTVariableExprNode [" << Name << "]" <<std::endl;
}

void ASTVariableExprNode::Accept(Visitor *v) {
    v->visit(this);
}