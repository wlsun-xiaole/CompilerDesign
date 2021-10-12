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

    const char *getNameCString() const;
    int getDimensionNum();
    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    bool checkIndexType(uint32_t&, uint32_t&);
    void updateDirty();

  private:
    const std::string name;
    Exprs indices;
};

#endif
