#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/CompoundStatement.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           std::vector<ExpressionNode*> *p_exps,
           std::vector<CompoundStatementNode> *p_stats);
    ~IfNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: expression, compound statement, compound statement
    std::vector<ExpressionNode*> exps;
    std::vector<CompoundStatementNode> stats;
};

#endif
