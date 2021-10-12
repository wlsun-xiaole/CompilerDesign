#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"

class ConstantValueNode : public ExpressionNode {
  public:
 //   ConstantValueNode(const uint32_t line, const uint32_t col);
    ConstantValueNode(const uint32_t line, const uint32_t col, union ConstVal const_val, VarType type_v);
    ConstantValueNode(ConstValPack* p_pack);
    ~ConstantValueNode() = default;

    void accept(AstNodeVisitor &p_visitor);
    void print() override;
    std::string getValueCString() const;
  private:
    // TODO: constant value
    union ConstVal value;
    VarType type;
};

#endif
