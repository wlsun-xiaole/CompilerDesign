#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           char* p_name, std::vector<ExpressionNode*>* p_exp);
    ~FunctionInvocationNode() = default;

    std::string getNameCString() const;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: function name, expressions
    std::string name;
    std::vector<ExpressionNode*> exps;
};

#endif
