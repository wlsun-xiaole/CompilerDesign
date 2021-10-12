#include "AST/expression.hpp"

ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}

PTypeSharedPtr ExpressionNode::getType() { return type; }
void ExpressionNode::setType(PTypeSharedPtr p) { type = p; }
void ExpressionNode::setDirty(int idx) {
	dirty[idx] = true;
}

bool ExpressionNode::isDirty(int idx) {
	return dirty[idx];
}

bool ExpressionNode::isAnyDirty() {
	bool flag = false;
	for(int i=0; i<6; i++)
		flag = (flag || dirty[i]);
	return flag;
}
/*
void ExpressionNode::setDimDiff(int diff) {
    dim_diff = diff;
}
int ExpressionNode::getDimDiff() {
    return dim_diff;
}*/

