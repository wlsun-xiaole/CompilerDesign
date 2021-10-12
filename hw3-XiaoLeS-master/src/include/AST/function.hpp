#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/CompoundStatement.hpp"

class FunctionNode : public AstNode {
  public:
/*    FunctionNode(const uint32_t line, const uint32_t col,
                 char* p_name, std::vector<DeclNode>* p_decls,
                 struct VarTypePack* ret_type_pack);
*/    FunctionNode(const uint32_t line, const uint32_t col,
                 char* p_name, std::vector<DeclNode>* p_decls,
                 struct VarTypePack* ret_type_pack,
                 CompoundStatementNode* compound_stat);
    ~FunctionNode() = default;

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
    void accept(AstNodeVisitor &p_visitor);
    std::string getNameCString() const;
    std::string getTypeCString() const;
  private:
    // TODO: name, declarations, return type, compound statement
    std::string name;
    std::vector<DeclNode> decls;
    struct VarTypePack ret_type;
    CompoundStatementNode* p_compound_stat;
};

#endif
