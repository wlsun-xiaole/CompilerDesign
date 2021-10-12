#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
							   VariableReferenceNode* p_var_ref,
                   			   std::vector<ExpressionNode*>* p_exps)
    : AstNode{line, col}, var_ref(*p_var_ref), exps(*p_exps) {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
     // TODO
	// Variable Reference
	var_ref.accept(p_visitor);

	// Expression Part

	ExpressionNode* p_exp = exps[0];
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

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}