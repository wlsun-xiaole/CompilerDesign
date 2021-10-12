#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             char* p_name,
                                             std::vector<ExpressionNode*> *p_exps)
    : ExpressionNode{line, col}, name(p_name), exps(*p_exps) {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
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

std::string VariableReferenceNode::getNameCString() const { return name; }

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}
