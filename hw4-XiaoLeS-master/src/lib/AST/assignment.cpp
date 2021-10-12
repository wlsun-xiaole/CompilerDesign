#include "AST/assignment.hpp"
#include "visitor/AstNodeVisitor.hpp"

AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               VariableReferenceNode *p_lvalue,
                               ExpressionNode *p_expr)
    : AstNode{line, col}, lvalue(p_lvalue), expr(p_expr) {}

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    lvalue->accept(p_visitor);
    expr->accept(p_visitor);
}

bool AssignmentNode::isAnyDirtyLChild() {
	return lvalue->isAnyDirty();
}
bool AssignmentNode::isAnyDirtyRChild() {
	return expr->isAnyDirty();
}
const char* AssignmentNode::getLChildPTypeCString() {
	return lvalue->getType()->getPTypeCString();
}
bool AssignmentNode::isLChildScalar(int& line, int& col) {
	const char* tar = lvalue->getType()->getPTypeCString();
	line = lvalue->getLocation().line;
	col = lvalue->getLocation().col;
	if(strcmp(tar, "integer") == 0 || strcmp(tar, "real") == 0 || strcmp(tar, "string") == 0 || strcmp(tar, "boolean") == 0) {
		return true;
	}
	return false;
}
const char* AssignmentNode::getRChildPTypeCString() {
	return expr->getType()->getPTypeCString();
}
bool AssignmentNode::isRChildScalar(int& line, int& col) {
	const char* tar = expr->getType()->getPTypeCString();
	line = expr->getLocation().line;
	col = expr->getLocation().col;
	if(strcmp(tar, "integer") == 0 || strcmp(tar, "real") == 0 || strcmp(tar, "string") == 0 || strcmp(tar, "boolean") == 0) {
		return true;
	}
	return false;
}

const char* AssignmentNode::getLChildNameCString() {
	return lvalue->getNameCString();
}

void AssignmentNode::setLChildDirty(int idx) {
	lvalue->setDirty(idx);
}