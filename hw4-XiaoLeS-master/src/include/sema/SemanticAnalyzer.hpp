#ifndef __SEMA_SEMANTIC_ANALYZER_H
#define __SEMA_SEMANTIC_ANALYZER_H

#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolManager.hpp"
#include "sema/ContextManager.hpp"
#include "cstring"

class SemanticAnalyzer : public AstNodeVisitor {
  public:
    SemanticAnalyzer() = default;
    ~SemanticAnalyzer() = default;

    void visit(ProgramNode &p_program) override;
    void visit(DeclNode &p_decl) override;
    void visit(VariableNode &p_variable) override;
    void visit(ConstantValueNode &p_constant_value) override;
    void visit(FunctionNode &p_function) override;
    void visit(CompoundStatementNode &p_compound_statement) override;
    void visit(PrintNode &p_print) override;
    void visit(BinaryOperatorNode &p_bin_op) override;
    void visit(UnaryOperatorNode &p_un_op) override;
    void visit(FunctionInvocationNode &p_func_invocation) override;
    void visit(VariableReferenceNode &p_variable_ref) override;
    void visit(AssignmentNode &p_assignment) override;
    void visit(ReadNode &p_read) override;
    void visit(IfNode &p_if) override;
    void visit(WhileNode &p_while) override;
    void visit(ForNode &p_for) override;
    void visit(ReturnNode &p_return) override;

    char** file_copy = NULL;
    bool hasErr = false;
    bool showTable();
    void setOptTable(int);
    void printErrRedecl(const char* sym_name, int line, int column);
    void printErrVariableDecl(const char* sym_name, int line, int column);
    void printErrVariableRef1(const char* sym_name, int line, int column);
    void printErrVariableRef2(const char* sym_name, int line, int column);
    void printErrVariableRef3(int line, int column);
    void printErrVariableRef4(const char* sym_name, int line, int column);
    void printErrBinOp(const char* op, int line, int col, const char* R, const char* L);
    void printErrUnOp(const char* op, int line, int col, const char* Operand);
    void printErrFuncInvoc1(const char* sym_name, int line, int column);
    void printErrFuncInvoc2(const char* sym_name, int line, int column);
    void printErrFuncInvoc3(const char* sym_name, int line, int column);
    void printErrFuncInvoc4(int line, int column, std::string &a, std::string &b);
    void printErrPrint(int line, int column);
    void printErrRead1(int line, int column);
    void printErrRead2(int line, int column);
    void printErrAssign1(int line, int column);
    void printErrAssign2(int line, int column, const char* sym_name);
    void printErrAssign3(int line, int column);
    void printErrAssign4(int line, int column);
    void printErrAssign5(int line, int column, const char* t1, const char* t2);
    void printErrIf(int line, int column);
    void printErrWhile(int line, int column);
    void printErrFor(int line, int column);
    void printErrReturn1(int line, int column);
    void printErrReturn2(int line, int column, const char* ret, const char* expr);


private:
    // TODO: something like symbol manager (manage symbol tables)
    //       context manager, return type manager
    SymbolManager smanager;
    ContextManager cmanager;
    int opt_table = 1;
   // TypeManager tmanager;
};

#endif
