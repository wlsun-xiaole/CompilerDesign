#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
			   std::vector<ExpressionNode*> *p_exps,
               std::vector<CompoundStatementNode> *p_stats)
    : AstNode{line, col}, exps(*p_exps), stats(*p_stats) {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    // an Expression
    if(!exps.empty()) {
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

	// one or two compound statements
	for (auto &stat : stats) {
		stat.accept(p_visitor);
	}
}

void IfNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}