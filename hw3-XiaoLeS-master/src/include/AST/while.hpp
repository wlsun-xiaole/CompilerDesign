#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              std::vector<ExpressionNode*> *p_exps,
              CompoundStatementNode* p_comp_stat);
    ~WhileNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: expression, compound statement
    std::vector<ExpressionNode*> exps;
    CompoundStatementNode* comp_stat;
};

#endif
