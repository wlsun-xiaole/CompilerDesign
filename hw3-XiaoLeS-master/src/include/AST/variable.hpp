#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "AST/ConstantValue.hpp"

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 const char* p_name, struct VarTypePack *vartype);
    VariableNode(const uint32_t line, const uint32_t col,
                 const char* p_name, struct ConstValPack* const_val_pack);
    ~VariableNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    std::string getNameCString() const;
    std::string getTypeCString() const;
    void accept(AstNodeVisitor &);
  private:
    // TODO: variable name, type, constant value
    const std::string name;
    struct VarTypePack var_type;
    ConstantValueNode* const_val_node;
};

#endif
