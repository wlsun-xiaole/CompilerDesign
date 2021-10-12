#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"

#include <memory>
#include <vector>

class VariableReferenceNode : public ExpressionNode {
  public:
    typedef std::vector<std::unique_ptr<ExpressionNode>> Exprs;

    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *p_name);
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char *p_name, Exprs *p_indices);
    ~VariableReferenceNode() = default;

    const std::string &getName() const;
    const char *getNameCString() const;

    const Exprs &getIndices() const;

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;

    void setPushAddr() { push_addr = true; }
    void unsetPushAddr() { push_addr = false; }
    bool isPushAddr() { return push_addr; }
    void setAddr(int addr) { cg_addr = addr; }
    int getAddr() { return cg_addr; }

  private:
    const std::string name;
    Exprs indices;
    bool push_addr = false;
    int cg_addr = 0;
};

#endif
