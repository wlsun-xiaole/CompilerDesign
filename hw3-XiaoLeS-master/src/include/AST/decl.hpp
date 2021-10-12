#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include <vector>
#include "AST/ast.hpp"
#include "AST/variable.hpp"



class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             std::vector<IdPack>* identifiers, struct VarTypePack* vartype);

    // constant variable declaration
    DeclNode(const uint32_t, const uint32_t col,
             std::vector<IdPack>* identifiers, struct ConstValPack* const_value_pack);

    ~DeclNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &);
    std::string getTypeForPara() const;

  private:
    // TODO: variables
    std::vector<VariableNode> variables;

};

#endif
