#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include <vector>
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"


class ProgramNode : public AstNode {
  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *p_ret_type,
                std::vector<DeclNode>* p_decls,
                std::vector<FunctionNode> *p_funcs,
                CompoundStatementNode *comp_stat
                /* TODO: return type, declarations, functions,
                 *       compound statement */);
    ~ProgramNode() = default;

    // visitor pattern version: 
    std::string getNameCString() const; 

    void print() override;
    void visitChildNodes(AstNodeVisitor &p_visitor);
  private:
    const std::string name;
    // TODO: return type, declarations, functions, compound statement
    const std::string ret_type;
    std::vector<DeclNode> list_decl;
    std::vector<FunctionNode> list_func;
    CompoundStatementNode comp_stat;
};

#endif
