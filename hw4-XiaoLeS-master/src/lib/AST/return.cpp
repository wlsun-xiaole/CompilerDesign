#include "AST/return.hpp"
#include "visitor/AstNodeVisitor.hpp"

ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       ExpressionNode *p_retval)
    : AstNode{line, col}, retval(p_retval) {}

void ReturnNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    retval->accept(p_visitor);
}

bool ReturnNode::isAnyDirtyChild() {
	return retval->isAnyDirty();
}

const char* ReturnNode::getTypeCString() {
	return retval->getType()->getPTypeCString();
}

void ReturnNode::getChildLocation(int& cline, int& ccol) {
	cline = retval->getLocation().line;
	ccol = retval->getLocation().col;
}
