#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          char* p_name, std::vector<ExpressionNode*>* p_exps);
    // array reference
    //VariableReferenceNode(const uint32_t line, const uint32_t col
    //                       /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    std::string getNameCString() const;
    void accept(AstNodeVisitor &p_visitor);

  private:
    // TODO: variable name, expressions
    std::string name;
    std::vector<ExpressionNode*> exps;
};

#endif
