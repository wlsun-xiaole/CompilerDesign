#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/VariableReference.hpp"

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
             VariableReferenceNode* p_var_ref);
    ~ReadNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
  private:
    // TODO: variable reference
    VariableReferenceNode var_ref;
};

#endif
