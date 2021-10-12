#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
									 OpType op_type_in, 
									 std::vector<ExpressionNode*>* p_exps)
    : ExpressionNode{line, col}, op_type(op_type_in), exps(*p_exps) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
	
	if (!exps.empty()) {

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
}

void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

std::string UnaryOperatorNode::getOpCString() {
	std::string str;

	switch(op_type) {
		case OpType_MINUS:
			str += "neg";
			break;
		case OpType_NOT:
			str += "not";
			break;
	}
	return str;
}