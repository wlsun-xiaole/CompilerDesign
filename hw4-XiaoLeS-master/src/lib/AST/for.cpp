#include "AST/for.hpp"
#include "visitor/AstNodeVisitor.hpp"

ForNode::ForNode(const uint32_t line, const uint32_t col, DeclNode *p_var_decl,
                 AssignmentNode *p_initial_statement,
                 ConstantValueNode *p_condition, CompoundStatementNode *p_body, int st, int ed)
    : AstNode{line, col}, var_decl(p_var_decl),
      initial_statement(p_initial_statement), condition(p_condition),
      body(p_body), start_val(st), end_val(ed) {}

void ForNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    var_decl->accept(p_visitor);
    if(var_decl->variables()[0]->isAnyDirty()) {
    	initial_statement->setLChildDirty(0);;
    }
    initial_statement->accept(p_visitor);
    condition->accept(p_visitor);
    body->accept(p_visitor);
}

int ForNode::getStartVal() { return start_val; }
int ForNode::getEndVal() {return end_val; }