#include "AST/print.hpp"
#include "visitor/AstNodeVisitor.hpp"

PrintNode::PrintNode(const uint32_t line, const uint32_t col,
                     ExpressionNode *p_expr)
    : AstNode{line, col}, target(p_expr) {}

void PrintNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    target->accept(p_visitor);
}

bool PrintNode::isAnyDirtyChild() {
	return target->isAnyDirty();
}

bool PrintNode::isChildScalar(int& line, int& col) {
	const char* tar = target->getType()->getPTypeCString();

	if(strcmp(tar, "integer") == 0 || strcmp(tar, "real") == 0 || strcmp(tar, "string") == 0 || strcmp(tar, "boolean") == 0) {
		return true;
	}
	line = target->getLocation().line;
	col = target->getLocation().col;
	return false;
}
