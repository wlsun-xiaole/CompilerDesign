#include "AST/while.hpp"
#include "visitor/AstNodeVisitor.hpp"

WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     ExpressionNode *p_condition, CompoundStatementNode *p_body)
    : AstNode{line, col}, condition(p_condition), body(p_body) {}

void WhileNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    condition->accept(p_visitor);
    body->accept(p_visitor);
}

bool WhileNode::isAnyDirtyChild() {
	return condition->isAnyDirty();
}

PTypeSharedPtr WhileNode::getChildTypeAndLocation(int& line, int& col) {
	line = condition->getLocation().line;
	col = condition->getLocation().col;
	return condition->getType();
}