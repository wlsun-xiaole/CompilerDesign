#include "AST/BinaryOperator.hpp"
#include "visitor/AstNodeVisitor.hpp"

BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                       Operator op, ExpressionNode *p_left,
                                       ExpressionNode *p_right)
    : ExpressionNode{line, col}, op(op), left(p_left), right(p_right) {}

const char *BinaryOperatorNode::getOpCString() const {
    return kOpString[static_cast<size_t>(op)];
}

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    left->accept(p_visitor);
    right->accept(p_visitor);
}

PTypeSharedPtr BinaryOperatorNode::getRightType() {
	return right->getType();
}

PTypeSharedPtr BinaryOperatorNode::getLeftType() {
	return left->getType();
}

void BinaryOperatorNode::setType(PTypeSharedPtr ptr) {
	type = ptr;
}

Operator BinaryOperatorNode::getOp() { return op; }

void BinaryOperatorNode::updateDirty() {
	for (int i=0; i<6 ;i++) {
		dirty[i] = (left->isDirty(i) || right->isDirty(i));
	}
}
/*
int BinaryOperatorNode::getRightDimDiff() {
	return right->getDimDiff();
}

int BinaryOperatorNode::getLeftDimDiff() {
	return left->getDimDiff();
}*/