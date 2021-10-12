#ifndef __SEMA_SEMANTIC_ANALYZER_H
#define __SEMA_SEMANTIC_ANALYZER_H

#include "cstdio"
#include "sema/SymbolTable.hpp"
#include "visitor/AstNodeVisitor.hpp"

class SemanticAnalyzer : public AstNodeVisitor {
  public:
    SemanticAnalyzer(const bool opt_dmp, const char *in_file_name, const char *out_file_name);
    ~SemanticAnalyzer();

    // Code Generator
    void dumpInstrs(const char *format, ...);
    void dumpFunc(const char *name);
    void dumpPushASymbol(const char *name);
    void dumpPopASymbol(const char *name);
    void dumpPushInteger(int reg);
    void dumpPopInteger(int reg);
    void dumpPopIntegerToA(int reg);
    void dumpAssignInteger(int src_reg, int dest_reg);

    void dumpPushReal(int reg);
    void dumpPopReal(int reg);
    void dumpPopRealToFA(int reg);
    void dumpAssignReal(int src_reg, int dest_reg);

    void openBufferFile();
    void closeBufferFile();
    FILE *buf_fp;
    int insert_pos;

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

    int label_num = 0;

  private:
    friend SymbolEntry *addSymbolFromVariableNode(SemanticAnalyzer &analyzer,
                                                  VariableNode &p_variable);
    friend bool isInForLoop(SemanticAnalyzer &analyzer);
    friend bool isInLocal(SemanticAnalyzer &analyzer);
    friend bool isInFunction(SemanticAnalyzer &analyzer);
    friend bool isInMain(SemanticAnalyzer &analyzer);

  private:
    enum class SemanticContext : uint8_t {
        kGlobal,
        kFunction,
        kForLoop,
        kLocal
    };

    SymbolManager symbol_manager;

    // record the context of current scope for performing special behavior
    std::vector<SemanticContext> context_stack;
    std::vector<const PType *> return_type_stack;

  // Code Generator
  private:
    const char *in_file_name;
    FILE *out_fp;
    int cg_addr_cnt;
    int cg_para_cnt;
};

#endif
