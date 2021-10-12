#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/VariableReference.hpp"
#include "AST/ast.hpp"
#include "AST/expression.hpp"

#include <memory>

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *p_lvalue, ExpressionNode *p_expr);
    ~AssignmentNode() = default;

    const VariableReferenceNode *getLvalue() const;
    const ExpressionNode *getExpr() const;

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    void setLValuePushAddr() { lvalue->setPushAddr(); }
    void unsetLValuePushAddr() { lvalue->unsetPushAddr(); }
    int getLvalueAddr() { return lvalue->getAddr(); }
    bool isExprConstant();
    
  private:
    std::unique_ptr<VariableReferenceNode> lvalue;
    std::unique_ptr<ExpressionNode> expr;
};

#endif
