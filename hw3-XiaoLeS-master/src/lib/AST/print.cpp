#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, ExpressionNode* exp)
    : AstNode{line, col} {
    	this->p_exp = exp;
    }

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    ConstantValueNode* cvp = dynamic_cast<ConstantValueNode *>(p_exp);
    if (cvp != NULL) {
    	cvp->accept(p_visitor);
    }

    BinaryOperatorNode *bop = dynamic_cast<BinaryOperatorNode *>(p_exp);
    if (bop != NULL) {
    	bop->accept(p_visitor);
    }

    UnaryOperatorNode *uop = dynamic_cast<UnaryOperatorNode *>(p_exp);
    if (uop != NULL) {
    	uop->accept(p_visitor);
    }

    FunctionInvocationNode *fip = dynamic_cast<FunctionInvocationNode *>(p_exp);
    if (fip != NULL) {
        fip->accept(p_visitor);
    }

    VariableReferenceNode *vrp = dynamic_cast<VariableReferenceNode *>(p_exp);
    if (vrp != NULL) {
        vrp->accept(p_visitor);
    }    
}

void PrintNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}
