#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/VariableReference.hpp"
#include "AST/ast.hpp"

#include <memory>
#include <cstring>

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col,
             VariableReferenceNode *p_target);
    ~ReadNode() = default;

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    bool isAnyDirtyChild();
    bool isChildScalar(int& line, int& col);
    const char* getChildNameCString();
  private:
    std::unique_ptr<VariableReferenceNode> target;
};

#endif
