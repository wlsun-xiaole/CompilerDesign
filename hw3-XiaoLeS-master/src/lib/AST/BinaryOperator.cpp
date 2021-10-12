#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
	OpType op_type_in, std::vector<ExpressionNode*>* p_exps)
    : ExpressionNode{line, col}, op_type(op_type_in), exps(*p_exps) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
     // TODO
	for (auto& p_exp : exps) {
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

void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

std::string BinaryOperatorNode::getOpCString() {
	std::string str;

	switch(op_type) {
		case OpType_MULTIPLY:
			str += "*";
			break;
		case OpType_DIVIDE:
			str += "/";
			break;
		case OpType_MOD:
			str += "mod";
			break;
		case OpType_PLUS:
			str += "+";
			break;
		case OpType_MINUS:
			str += "-";
			break;
		case OpType_LESS:
			str += "<";
			break;
		case OpType_LESS_OR_EQUAL:
			str += "<=";
			break;
		case OpType_GREATER:
			str += ">";
			break;
		case OpType_GREATER_OR_EQUAL:
			str += ">=";
			break;
		case OpType_EQUAL:
			str += "=";
			break;
		case OpType_NOT_EQUAL:
			str += "<>";
			break;
		case OpType_AND:
			str += "and";
			break;
		case OpType_OR:
			str += "or";
			break;
		case OpType_NOT:
			str += "not";
			break;
	}
	return str;
}
