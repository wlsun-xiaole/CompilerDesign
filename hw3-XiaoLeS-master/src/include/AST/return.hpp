#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"

class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col,
               std::vector<ExpressionNode*> *p_exps);
    ~ReturnNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: expression
    std::vector<ExpressionNode*> exps;
};

#endif
