#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      OpType op_type, std::vector<ExpressionNode*>* p_exps);
    ~UnaryOperatorNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
    std::string getOpCString();
  private:
    // TODO: operator, expression
    OpType op_type;
    std::vector<ExpressionNode*> exps;
};

#endif
