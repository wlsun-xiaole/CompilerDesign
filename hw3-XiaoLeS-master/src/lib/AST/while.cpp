#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
	             	 std::vector<ExpressionNode*> *p_exps,
              		 CompoundStatementNode* p_comp_stat)
    : AstNode{line, col}, exps(*p_exps) {
    	comp_stat = p_comp_stat;
    }

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
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

    // a Compound Statment
    comp_stat->accept(p_visitor);
}

void WhileNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}
