#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
	             DeclNode* p_decl_in, AssignmentNode* p_assig_in, 
            	 ConstantValueNode* p_const_in,
            	 CompoundStatementNode *p_comp_stat_in)
    : AstNode{line, col}, p_decl(p_decl_in), p_assig(p_assig_in),
    	p_const(p_const_in), p_comp_stat(p_comp_stat_in) {}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {}

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    p_decl->accept(p_visitor);
    p_assig->accept(p_visitor);
    p_const->accept(p_visitor);
    p_comp_stat->accept(p_visitor);
}

void ForNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}
