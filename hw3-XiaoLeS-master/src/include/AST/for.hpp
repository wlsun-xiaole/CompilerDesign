#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/assignment.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/CompoundStatement.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col,
            DeclNode* p_decl_in, AssignmentNode* p_assig_in, 
            ConstantValueNode* p_const_in,
            CompoundStatementNode *p_comp_stat_in);
    ~ForNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: declaration, assignment, expression, compound statement
    DeclNode* p_decl;
    AssignmentNode* p_assig;
    ConstantValueNode* p_const;
    CompoundStatementNode* p_comp_stat;
};

#endif
