#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *p_name, const char *p_ret_type,
                         std::vector<DeclNode> *p_decls,
                         std::vector<FunctionNode> *p_funcs,
                         CompoundStatementNode *p_comp_stat
                         )
    : AstNode{line, col}, name(p_name), ret_type(p_ret_type), 
      list_decl(*p_decls), list_func(*p_funcs), comp_stat(*p_comp_stat)
    //, list_func(list_func), comp_stat(comp_stat)
      {
        //printf("number of decl: %d\n", list_decl.size());
       // list_decl.assign(p_decls->begin(),p_decls->end());
      }

// visitor pattern version: 
std::string ProgramNode::getNameCString() const { return name; }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    // visitChildNodes();
    // decrementIndentation();
}


void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) { // visitor pattern version
//     /* TODO
//      *
//      * for (auto &decl : var_decls) {
//      *     decl->accept(p_visitor);
//      * }
//      *
//      * // functions
//      *
//      * body->accept(p_visitor);
//      */
    // declarations
    for (auto &decl : list_decl) {
        decl.accept(p_visitor);
    }

    // functions
    for (auto &func : list_func) {
        func.accept(p_visitor);
    }
    // compound statment
    comp_stat.accept(p_visitor);
}
