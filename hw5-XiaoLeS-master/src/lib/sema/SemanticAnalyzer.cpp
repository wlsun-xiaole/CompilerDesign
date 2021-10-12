#include "sema/SemanticAnalyzer.hpp"
#include "sema/error.hpp"
#include "visitor/AstNodeInclude.hpp"

#include <cassert>
#include <cstdarg>
#include <cstdio>

SemanticAnalyzer::SemanticAnalyzer(const bool opt_dmp, const char *in_file_name, const char *out_file_name)
    : symbol_manager(opt_dmp), in_file_name(in_file_name) {
    this->out_fp = fopen(out_file_name, "w+");
    assert(this->out_fp != NULL && "fopen() fails.\n");
}

SemanticAnalyzer::~SemanticAnalyzer() {
    fclose(this->out_fp);
}

void SemanticAnalyzer::openBufferFile() {
    buf_fp = fopen("a_tmp_file", "w+");
    assert(buf_fp != NULL && "fopen() fails.\n");

    // copy front
    fseek(out_fp, 0, SEEK_SET);
    char buf[1024];
    int cur_pos = ftell(out_fp);
    while(cur_pos != insert_pos) {
        fgets(buf , 1024, out_fp);
        fputs(buf, buf_fp);
        cur_pos = ftell(out_fp);
    }

    // swap
    FILE* tmp_fp = out_fp;
    out_fp = buf_fp;
    buf_fp = tmp_fp;
}
void SemanticAnalyzer::closeBufferFile() {
    // swap back
    FILE* tmp_fp = out_fp;
    out_fp = buf_fp;
    buf_fp = tmp_fp;

    char buf[1024];
    while(!feof(out_fp)) {
        fgets(buf , 1024, out_fp);
        fputs(buf, buf_fp);        
    }

    // buf -> out
    fseek(out_fp, 0, SEEK_SET);
    fseek(buf_fp, 0, SEEK_SET);
    while(!feof(buf_fp)) {
        fgets(buf , 1024 , buf_fp);
        fputs(buf, out_fp);  
    }
    fclose(buf_fp);
}

void SemanticAnalyzer::dumpInstrs(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->out_fp, format, args);
    va_end(args);
}

void SemanticAnalyzer::dumpFunc(const char *name) {
    dumpInstrs("%s%s%s%s%s%s%s%s",
        ".section    .text\n",
        "    .align 2\n",
        "    .global ",
        name, "\n",
        "    .type ",
        name,
        ", @function\n"
    );
}

void SemanticAnalyzer::dumpPushASymbol(const char *name) {
    // create label
    dumpInstrs("%s:\n", name);
    // stack operation
    dumpInstrs("    %s %s, %s, %s\n", "addi", "sp", "sp", "-128");
    dumpInstrs("    %s %s, %s(%s)\n", "sw", "ra", "124", "sp");
    dumpInstrs("    %s %s, %s(%s)\n", "sw", "s0", "120", "sp");
    dumpInstrs("    %s %s, %s, %s\n", "addi", "s0", "sp", "128");
}

void SemanticAnalyzer::dumpPopASymbol(const char *name) {
    dumpInstrs("    %s %s, %s(%s)\n", "lw", "ra", "124", "sp");
    dumpInstrs("    %s %s, %s(%s)\n", "lw", "s0", "120", "sp");
    dumpInstrs("    %s %s, %s, %s\n", "addi", "sp", "sp", "128");
    dumpInstrs("    %s %s\n", "jr", "ra");
    dumpInstrs("    .size %s, .-%s\n", name, name);    
}

const char *kRedeclaraedSymbolErrMsg = "symbol %s is redeclared";

void SemanticAnalyzer::visit(ProgramNode &p_program) {

    // global symbol table
    symbol_manager.pushGlobalScope();
    context_stack.emplace_back(SemanticContext::kGlobal);
    return_type_stack.emplace_back(p_program.getTypePtr());
    cg_addr_cnt = 0;
    cg_para_cnt = 0;

    auto success = symbol_manager.addSymbol(
        p_program.getName(), SymbolEntry::KindEnum::kProgramKind,
        p_program.getTypePtr(), static_cast<const Constant *>(nullptr));
    if (!success) {
        logSemanticError(p_program.getLocation(), kRedeclaraedSymbolErrMsg,
                         p_program.getNameCString());
    }

    // Code Generation
    dumpInstrs("%s%s%s%s", 
        "    .file \"", this->in_file_name, "\"\n",
        "    .option nopic\n"
    );
    insert_pos = ftell(out_fp);

    p_program.visitChildNodes(*this);

    // TODO: semantic checks

    return_type_stack.pop_back();
    context_stack.pop_back();
    symbol_manager.popScope();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

bool isInForLoop(SemanticAnalyzer &analyzer) {
    return analyzer.context_stack.back() ==
           SemanticAnalyzer::SemanticContext::kForLoop;
}

bool isInLocal(SemanticAnalyzer &analyzer) {
    return analyzer.context_stack.back() ==
           SemanticAnalyzer::SemanticContext::kLocal;
}

bool isInFunction(SemanticAnalyzer &analyzer) {
    return analyzer.context_stack.back() ==
           SemanticAnalyzer::SemanticContext::kFunction;
}

bool isInMain(SemanticAnalyzer &analyzer) {
    return analyzer.context_stack.back() == 
           SemanticAnalyzer::SemanticContext::kGlobal;
}

SymbolEntry *addSymbolFromVariableNode(SemanticAnalyzer &analyzer,
                                       VariableNode &p_variable) {
    SymbolEntry::KindEnum kind;
    const Constant *constant_ptr = p_variable.getConstantPtr();

    if (isInForLoop(analyzer)) {
        kind = SymbolEntry::KindEnum::kLoopVarKind;
    } else if (isInLocal(analyzer)) {
        kind = constant_ptr ? SymbolEntry::KindEnum::kConstantKind
                            : SymbolEntry::KindEnum::kVariableKind;
    } else if (isInFunction(analyzer)) {
        kind = SymbolEntry::KindEnum::kParameterKind;
    } else { // global scope
        kind = constant_ptr ? SymbolEntry::KindEnum::kConstantKind
                            : SymbolEntry::KindEnum::kVariableKind;
    }

    auto *entry = analyzer.symbol_manager.addSymbol(
        p_variable.getName(), kind, p_variable.getTypePtr(), constant_ptr);
    if (!entry) {
        logSemanticError(p_variable.getLocation(), kRedeclaraedSymbolErrMsg,
                         p_variable.getNameCString());
    }

    return entry;
}

void SemanticAnalyzer::dumpPushInteger(int reg) {
    dumpInstrs("    addi sp, sp, -4\n");
    dumpInstrs("    sw t%d, 0(sp)\n", reg);
}

void SemanticAnalyzer::dumpPopInteger(int reg) {
    dumpInstrs("    lw t%d, 0(sp)\n", reg);
    dumpInstrs("    addi sp, sp, 4\n");    
}

void SemanticAnalyzer::dumpPopIntegerToA(int reg) {
    dumpInstrs("    lw a%d, 0(sp)\n", reg);
    dumpInstrs("    addi sp, sp, 4\n");        
}

void SemanticAnalyzer::dumpAssignInteger(int src_reg, int dest_reg) {
    dumpInstrs("    sw t%d, 0(t%d)\n", src_reg, dest_reg);
}

void SemanticAnalyzer::dumpPushReal(int reg) {
    dumpInstrs("    addi sp, sp, -4\n");
    dumpInstrs("    fsw ft%d, 0(sp)\n", reg);
}

void SemanticAnalyzer::dumpPopReal(int reg) {
    dumpInstrs("    flw ft%d, 0(sp)\n", reg);
    dumpInstrs("    addi sp, sp, 4\n");    
}

void SemanticAnalyzer::dumpPopRealToFA(int reg) {
    dumpInstrs("    flw fa%d, 0(sp)\n", reg);
    dumpInstrs("    addi sp, sp, 4\n");        
}

void SemanticAnalyzer::dumpAssignReal(int src_reg, int dest_reg) {
    dumpInstrs("    fsw ft%d, 0(t%d)\n", src_reg, dest_reg);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {

    auto *entry = addSymbolFromVariableNode(*this, p_variable);

    // Global
    if (isInMain(*this)) {
        if (entry->getKind() == SymbolEntry::KindEnum::kVariableKind) {
            // Integer
            dumpInstrs(".comm %s, %d, %d\n", entry->getNameCString(), 4, 4);
        } else if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind) {
            const Constant *constant = entry->getAttribute().constant();

            dumpInstrs(".section    .rodata\n");
            dumpInstrs("    .align 2\n");
            dumpInstrs("    .globl %s\n", entry->getNameCString());
            dumpInstrs("    .type %s, @object\n", entry->getNameCString());
            dumpInstrs("%s:\n", entry->getNameCString());
            // Integer
            if(entry->getTypePtr()->isInteger()) {
                dumpInstrs("    .word %d\n", constant->integer());
            } else if (entry->getTypePtr()->isBool()) {
                if(constant->boolean()) {
                    dumpInstrs("    .word %d\n", 1);
                } else {
                    dumpInstrs("    .word %d\n", 0);
                }
            } else if (entry->getTypePtr()->isString()) {

            } else {
                // REAL
            }


            p_variable.unsetPushConstantValue();

        } else {
            assert(false && "Shouldn't reach here!QQ");
        }

    } else if (entry->getKind() == SymbolEntry::KindEnum::kParameterKind) {
        entry->cg_addr = 12 + cg_addr_cnt*4;
        cg_addr_cnt++;
        if(cg_para_cnt < 8) {
            if(entry->getTypePtr()->isReal()){
                dumpInstrs("    fsw fa%d, -%d(s0)\n", cg_para_cnt, entry->cg_addr);
            } else {
                dumpInstrs("    sw a%d, -%d(s0)\n", cg_para_cnt, entry->cg_addr);
            }
        } else {
            if(entry->getTypePtr()->isReal()) {
                dumpInstrs("    fsw ft%d, -%d(s0)\n", cg_para_cnt-8, entry->cg_addr);
            } else {
                dumpInstrs("    sw t%d, -%d(s0)\n", cg_para_cnt-8, entry->cg_addr);
            }
        }
        cg_para_cnt++;
    } else {
    // Local
        if (entry->getKind() == SymbolEntry::KindEnum::kVariableKind || entry->getKind() == SymbolEntry::KindEnum::kLoopVarKind) {
            entry->cg_addr = 12 + cg_addr_cnt*4;

            std::vector<uint64_t> dims = entry->getTypePtr()->getDimensions();
            int num = 1;
            for(auto &dim : dims) {
                num *= dim;
            }

            cg_addr_cnt += num;
        } else if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind){
            if(entry->getTypePtr()->isInteger() || entry->getTypePtr()->isBool()) {
                entry->cg_addr = 12 + cg_addr_cnt*4;
                cg_addr_cnt++;

                dumpInstrs("    addi t0, s0, -%d\n", entry->cg_addr);
                dumpPushInteger(0);
            } 
        }
    }

    p_variable.visitChildNodes(*this);

    if (!entry) {
        return;
    }

    for (auto dimension : p_variable.getTypePtr()->getDimensions()) {
        if (dimension == 0) {
            entry->setError();

            logSemanticError(p_variable.getLocation(),
                             "'%s' declared as an array with an index that is "
                             "not greater than 0",
                             p_variable.getNameCString());
        }
    }

    // Code Generation    
    if (!isInMain(*this) && (entry->getKind() == SymbolEntry::KindEnum::kConstantKind)) {
        dumpPopInteger(0);
        dumpPopInteger(1);
        dumpAssignInteger(0,1);
    }
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    p_constant_value.setInferredType(
        p_constant_value.getTypePtr()->getStructElementType(0));
    // Integer
    if(p_constant_value.isPushValue()) {
        if(p_constant_value.getTypePtr()->isInteger()) {
            dumpInstrs("    li t0, %d\n", p_constant_value.getConstantPtr()->integer());
            dumpPushInteger(0);
        } else if(p_constant_value.getTypePtr()->isBool()) {
            dumpInstrs("    li t0, %d\n", p_constant_value.getConstantPtr()->boolean()? 1 : 0);
            dumpPushInteger(0);
        } else if(p_constant_value.getTypePtr()->isString()) {
            dumpInstrs("    .string    \"%s\"\n", p_constant_value.getConstantPtr()->string());
            closeBufferFile();
        } else {
            // Floating point
            dumpInstrs("    .float    %f\n", p_constant_value.getConstantPtr()->real());
            closeBufferFile();
        }
                
    }
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    auto success = symbol_manager.addSymbol(
        p_function.getName(), SymbolEntry::KindEnum::kFunctionKind,
        p_function.getTypePtr(), &p_function.getParameters());
    if (!success) {
        logSemanticError(p_function.getLocation(), kRedeclaraedSymbolErrMsg,
                         p_function.getNameCString());
    }

    context_stack.emplace_back(SemanticContext::kFunction);
    return_type_stack.emplace_back(p_function.getTypePtr());
    symbol_manager.pushScope();
    cg_addr_cnt = 0;
    cg_para_cnt = 0;

    dumpFunc(p_function.getNameCString());
    dumpPushASymbol(p_function.getNameCString());

    p_function.visitChildNodes(*this);

    // TODO: semantic checks

    symbol_manager.popScope();
    return_type_stack.pop_back();
    context_stack.pop_back();
    dumpPopASymbol(p_function.getNameCString());
    cg_addr_cnt = 0;
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {

    bool first_scope_of_program = isInMain(*this);
    if(first_scope_of_program) {
        // Code Genereate
        dumpFunc("main");
        dumpPushASymbol("main");
    }

    bool first_scope_of_function = isInFunction(*this);
    context_stack.emplace_back(SemanticContext::kLocal);
    if (!first_scope_of_function) {
        symbol_manager.pushScope();
    }

    p_compound_statement.visitChildNodes(*this);

    if (!first_scope_of_function) {
        symbol_manager.popScope();
    }


    if(first_scope_of_program) {
        // Code Genereate
        dumpPopASymbol("main");
    }
    context_stack.pop_back();
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    p_print.visitChildNodes(*this);

    auto *target_type = p_print.getTarget()->getInferredType();
    if (!target_type) {
        return;
    }

    if (!target_type->isScalar()) {
        logSemanticError(p_print.getTarget()->getLocation(),
                         "expression of print statement must be scalar type");
    }
    if(target_type->isString()) {
        dumpPopIntegerToA(0);
        dumpInstrs("    jal ra, printString\n");
    } else if(target_type->isReal()) {
        dumpPopRealToFA(0);
        dumpInstrs("    jal ra, printReal\n");
    } else {
        dumpPopIntegerToA(0);
        dumpInstrs("    jal ra, printInt\n");
    }
}

static bool checkOperandsInArithmeticOp(Operator op, const PType *left_type,
                                        const PType *right_type) {
    if (op == Operator::kPlusOp && left_type->isString() &&
        right_type->isString()) {
        return true;
    }

    if ((left_type->isInteger() || left_type->isReal()) &&
        (right_type->isInteger() || right_type->isReal())) {
        return true;
    }

    return false;
}

static bool checkOperandsInRelationOp(const PType *left_type,
                                      const PType *right_type) {
    if ((left_type->isInteger() || left_type->isReal()) &&
        (right_type->isInteger() || right_type->isReal())) {
        return true;
    }
    return false;
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    p_bin_op.visitChildNodes(*this);

    auto *left_type = p_bin_op.getLeftOperand()->getInferredType();
    auto *right_type = p_bin_op.getRightOperand()->getInferredType();
    if (!left_type || !right_type) {
        return;
    }

    // code generation
    int label_tmp;
    switch (p_bin_op.getOp()) {
    case Operator::kPlusOp:
        if(left_type->isReal() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fadd.s ft0, ft1, ft0\n");
            dumpPushReal(0);            
        } else if (left_type->isReal() && right_type->isInteger()){
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fadd.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fadd.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else {
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    add t0, t1, t0\n");
            dumpPushInteger(0);
        }
        break;
    case Operator::kMinusOp:
        if(left_type->isReal() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fsub.s ft0, ft1, ft0\n");
            dumpPushReal(0);            
        } else if (left_type->isReal() && right_type->isInteger()){
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fsub.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fsub.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else {
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    sub t0, t1, t0\n");
            dumpPushInteger(0);
        }
        break;
    case Operator::kMultiplyOp:
        if(left_type->isReal() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fmul.s ft0, ft1, ft0\n");
            dumpPushReal(0);            
        } else if (left_type->isReal() && right_type->isInteger()){
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fmul.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fmul.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else {
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    mul t0, t1, t0\n");
            dumpPushInteger(0);
        }
        break;
    case Operator::kDivideOp:
        if(left_type->isReal() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fdiv.s ft0, ft1, ft0\n");
            dumpPushReal(0);            
        } else if (left_type->isReal() && right_type->isInteger()){
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fdiv.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fdiv.s ft0, ft1, ft0\n");
            dumpPushReal(0);
        } else {
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    div t0, t1, t0\n");
            dumpPushInteger(0);
        }

        break;
    case Operator::kModOp:
        dumpPopInteger(0);
        dumpPopInteger(1);
        dumpInstrs("    rem t0, t1, t0\n");
        dumpPushInteger(0);
        break;
    case Operator::kAndOp:
        label_tmp = label_num;
        label_num++;
        dumpPopInteger(0);
        dumpPopInteger(1);
        dumpInstrs("    li t2, 0\n");
        dumpInstrs("beq t1, t2, L%d\n", label_tmp);
        dumpInstrs("beq t0, t2, L%d\n", label_tmp);
        dumpInstrs("    li t2, 1\n");
        dumpInstrs("L%d:\n", label_tmp);
        dumpPushInteger(2);
        break;
    case Operator::kOrOp:
        label_tmp = label_num;
        label_num++;
        dumpPopInteger(0);
        dumpPopInteger(1);
        dumpInstrs("    li t2, 1\n");
        dumpInstrs("beq t1, t2, L%d\n", label_tmp);
        dumpInstrs("beq t0, t2, L%d\n", label_tmp);
        dumpInstrs("    li t2, 0\n");
        dumpInstrs("L%d:\n", label_tmp);
        dumpPushInteger(2);
        break;
    case Operator::kLessOp:
        if(left_type->isReal() && right_type->isReal()){
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    flt.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isReal() && right_type->isInteger()) {
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    flt.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    flt.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    blt t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }
        break;
    case Operator::kLessOrEqualOp:
        if(left_type->isReal() && right_type->isReal()){
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fle.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isReal() && right_type->isInteger()) {
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fle.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fle.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    ble t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }
        break;
    case Operator::kEqualOp:
        if(left_type->isReal() && right_type->isReal()){
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isReal() && right_type->isInteger()) {
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpPushInteger(0);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    beq t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }
        break;
    case Operator::kGreaterOp:
        if(left_type->isReal() && right_type->isReal()){
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    fle.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else if (left_type->isReal() && right_type->isInteger()) {
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    fle.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    fle.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    bgt t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }
        break;
    case Operator::kGreaterOrEqualOp:
        if(left_type->isReal() && right_type->isReal()){
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    flt.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else if (left_type->isReal() && right_type->isInteger()) {
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    flt.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else if (left_type->isInteger() && right_type->isReal()) {
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    flt.s t0, ft0, ft1\n");
            dumpPushInteger(0);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    bge t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }    
        break;
    case Operator::kNotEqualOp:
        if(left_type->isReal() && right_type->isReal()){
            label_tmp = label_num;
            label_num += 2;
            dumpPopReal(0);
            dumpPopReal(1);
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpInstrs("    li t1, 1\n");
            dumpInstrs("    beq t0, t1, L%d\n", label_tmp);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("   li t1, 0\n");
            dumpInstrs("L%d:\n", label_tmp+1);
            dumpPushInteger(1);
        } else if (left_type->isReal() && right_type->isInteger()) {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopReal(1);
            dumpInstrs("    fcvt.s.w ft0, t0\n");
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpInstrs("    beq t0, t1, L%d\n", label_tmp);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("   li t1, 0\n");
            dumpInstrs("L%d:\n", label_tmp+1);
            dumpPushInteger(1);
        } else if (left_type->isInteger() && right_type->isReal()) {
            label_tmp = label_num;
            label_num += 2;
            dumpPopReal(0);
            dumpPopInteger(1);
            dumpInstrs("    fcvt.s.w ft1, t1\n");
            dumpInstrs("    feq.s t0, ft1, ft0\n");
            dumpInstrs("    beq t0, t1, L%d\n", label_tmp);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("   li t1, 0\n");
            dumpInstrs("L%d:\n", label_tmp+1);
            dumpPushInteger(1);
        } else {
            label_tmp = label_num;
            label_num += 2;
            dumpPopInteger(0);
            dumpPopInteger(1);
            dumpInstrs("    beq t1, t0, L%d\n", label_tmp);
            dumpInstrs("    li t0, 0\n");
            dumpPushInteger(0);
            dumpInstrs("    j L%d\n", label_tmp+1);
            dumpInstrs("L%d:\n", label_tmp);
            dumpInstrs("    li t0, 1\n");
            dumpPushInteger(0);
            dumpInstrs("L%d:\n", label_tmp+1);
        }
        break;
    default:
        assert(false &&
               "Shouldn't reach here (there is an unknown op or unary op)");
    }

    switch (p_bin_op.getOp()) {
    case Operator::kPlusOp:
    case Operator::kMinusOp:
    case Operator::kMultiplyOp:
    case Operator::kDivideOp:
        if (!checkOperandsInArithmeticOp(p_bin_op.getOp(), left_type,
                                         right_type)) {
            break;
        }

        if (left_type->isString()) {
            p_bin_op.setInferredType(
                new PType(PType::PrimitiveTypeEnum::kStringType));
        } else if (left_type->isReal() || right_type->isReal()) {
            p_bin_op.setInferredType(
                new PType(PType::PrimitiveTypeEnum::kRealType));
        } else {
            p_bin_op.setInferredType(
                new PType(PType::PrimitiveTypeEnum::kIntegerType));
        }
        return;
    case Operator::kModOp:
        if (!(left_type->isInteger() && right_type->isInteger())) {
            break;
        }
        p_bin_op.setInferredType(
            new PType(PType::PrimitiveTypeEnum::kIntegerType));
        return;
    case Operator::kAndOp:
    case Operator::kOrOp:
        if (!(left_type->isBool() && right_type->isBool())) {
            break;
        }
        p_bin_op.setInferredType(
            new PType(PType::PrimitiveTypeEnum::kBoolType));
        return;
    case Operator::kLessOp:
    case Operator::kLessOrEqualOp:
    case Operator::kEqualOp:
    case Operator::kGreaterOp:
    case Operator::kGreaterOrEqualOp:
    case Operator::kNotEqualOp:
        if (!checkOperandsInRelationOp(left_type, right_type)) {
            break;
        }
        p_bin_op.setInferredType(
            new PType(PType::PrimitiveTypeEnum::kBoolType));
        return;
    default:
        assert(false &&
               "Shouldn't reach here (there is an unknown op or unary op)");
    }

    logSemanticError(p_bin_op.getLocation(),
                     "invalid operands to binary operator '%s' ('%s' and '%s')",
                     p_bin_op.getOpCString(), left_type->getPTypeCString(),
                     right_type->getPTypeCString());
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    p_un_op.visitChildNodes(*this);

    auto *operand_type = p_un_op.getOperand()->getInferredType();
    if (!operand_type) {
        return;
    }
    int label_tmp;

    switch (p_un_op.getOp()) {
    case Operator::kNegOp:
        if (operand_type->isInteger()) {
            p_un_op.setInferredType(
                new PType(PType::PrimitiveTypeEnum::kIntegerType));
            dumpPopInteger(0);
            dumpInstrs("    li t1, -1\n");
            dumpInstrs("    mul t0, t1, t0\n");
            dumpPushInteger(0);
        } else if (operand_type->isReal()) {
            p_un_op.setInferredType(
                new PType(PType::PrimitiveTypeEnum::kRealType));
        } else {
            break;
        }
        return;
    case Operator::kNotOp:
        if (!operand_type->isBool()) {
            break;
        }
        label_tmp = label_num;
        label_num ++;
        p_un_op.setInferredType(new PType(PType::PrimitiveTypeEnum::kBoolType));
        dumpPopInteger(0);
        dumpInstrs("    li t1, 0\n");
        dumpInstrs("    bne t0, t1, L%d\n", label_tmp);
        dumpInstrs("    li t1, 1\n");
        dumpInstrs("L%d:\n", label_tmp);
        dumpPushInteger(1);
        return;
    default:
        assert(false &&
               "Shouldn't reach here (there is an unknown op or unary op)");
    }

    logSemanticError(p_un_op.getLocation(),
                     "invalid operand to unary operator '%s' ('%s')",
                     p_un_op.getOpCString(), operand_type->getPTypeCString());
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    p_func_invocation.visitChildNodes(*this);

    auto *entry = symbol_manager.lookup(p_func_invocation.getName());
    if (!entry) {
        logSemanticError(p_func_invocation.getLocation(),
                         "use of undeclared symbol '%s'",
                         p_func_invocation.getNameCString());
        return;
    }

    if (entry->getKind() != SymbolEntry::KindEnum::kFunctionKind) {
        logSemanticError(p_func_invocation.getLocation(),
                         "call of non-function symbol '%s'",
                         p_func_invocation.getNameCString());
        return;
    }

    auto &arguments = p_func_invocation.getArguments();
    auto &parameters = *entry->getAttribute().parameters();
    if (arguments.size() != FunctionNode::getParametersNum(parameters)) {
        logSemanticError(p_func_invocation.getLocation(),
                         "too few/much arguments provided for function '%s'",
                         p_func_invocation.getNameCString());
        return;
    }

    FunctionInvocationNode::Exprs::const_iterator argument = arguments.begin();
    for (std::size_t i = 0; i < parameters.size(); ++i) {
        auto &variables = parameters[i]->getVariables();
        for (auto &variable : variables) {
            auto *expr_type = (*argument)->getInferredType();
            if (!expr_type) {
                return;
            }

            if (!expr_type->compare(variable->getTypePtr())) {
                logSemanticError(
                    (*argument)->getLocation(),
                    "incompatible type passing '%s' to parameter of type '%s'",
                    expr_type->getPTypeCString(),
                    variable->getTypePtr()->getPTypeCString());
                return;
            }
            argument++;
        }
    }

    p_func_invocation.setInferredType(
        new PType(entry->getTypePtr()->getPrimitiveType()));

    // code generation
    int para_num_now = arguments.size();
    int arg_idx = para_num_now-1;
    if (para_num_now > 8) {
        for(int para_num = para_num_now - 8; para_num > 0; para_num--) {
            
            if(arguments[arg_idx--]->getInferredType()->isReal()){
                dumpPopReal(para_num-1);
            } else {
                dumpPopInteger(para_num-1);
            }

        }

        for(int para_num = 8; para_num > 0; para_num--) {
            if(arguments[arg_idx--]->getInferredType()->isReal()){
                dumpPopRealToFA(para_num-1);
            } else {
                dumpPopIntegerToA(para_num-1);
            }
        }
    } else {
        for(int para_num = arguments.size(); para_num > 0; para_num--) {
            
            if(arguments[arg_idx--]->getInferredType()->isReal()){
                dumpPopRealToFA(para_num-1);
            } else {
                dumpPopIntegerToA(para_num-1);
            }
        }
    }
    // call function
    dumpInstrs("    jal ra, %s\n", p_func_invocation.getNameCString());
    // return value handling
    if(entry->getTypePtr()->isReal()){
        dumpInstrs("    fcvt.d.s ft0, fa0\n");
        dumpInstrs("    fcvt.s.d ft0, ft0\n");
        dumpPushReal(0);
    } else if (!entry->getTypePtr()->isVoid()) {
        dumpInstrs("    mv t0, a0\n");
        dumpPushInteger(0);
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    p_variable_ref.visitChildNodes(*this);

    auto *entry = symbol_manager.lookup(p_variable_ref.getName());


    if (entry == nullptr) {
        logSemanticError(p_variable_ref.getLocation(),
                         "use of undeclared symbol '%s'",
                         p_variable_ref.getNameCString());
        return;
    }

    if (!(entry->getKind() == SymbolEntry::KindEnum::kParameterKind ||
          entry->getKind() == SymbolEntry::KindEnum::kVariableKind ||
          entry->getKind() == SymbolEntry::KindEnum::kLoopVarKind ||
          entry->getKind() == SymbolEntry::KindEnum::kConstantKind)) {
        logSemanticError(p_variable_ref.getLocation(),
                         "use of non-variable symbol '%s'",
                         p_variable_ref.getNameCString());
        return;
    }

    if (entry->hasError()) {
        return;
    }

    for (auto &index : p_variable_ref.getIndices()) {
        if (index->getInferredType() == nullptr) {
            return;
        }

        if (!index->getInferredType()->isInteger()) {
            logSemanticError(index->getLocation(),
                             "index of array reference must be an integer");
            return;
        }
    }

    auto *inferred_type = entry->getTypePtr()->getStructElementType(
        p_variable_ref.getIndices().size());
    if (!inferred_type) {
        logSemanticError(p_variable_ref.getLocation(),
                         "there is an over array subscript on '%s'",
                         p_variable_ref.getNameCString());
        return;
    }

    p_variable_ref.setInferredType(inferred_type);


    // Code Generation
    // addr
    if (p_variable_ref.isPushAddr()) {
        if (entry->getLevel() == 0) {
        // Global
            dumpInstrs("    la t0, %s\n", entry->getNameCString());
            dumpPushInteger(0);
        } else {
        // Local
            int dim_size = p_variable_ref.getIndices().size();

            // Integer
            if (dim_size == 0) {
                dumpInstrs("    addi t0, s0, -%d\n", entry->cg_addr); 
                dumpPushInteger(0); 
                // for loopvar
                p_variable_ref.setAddr(entry->cg_addr);
            } else {
                std::vector<uint64_t> dims = entry->getTypePtr()->getDimensions();
                dumpPopInteger(0);
                // get indices from stack
                for (int i=1; i<dim_size ;i++) {
                    dumpPopInteger(1);
                    dumpInstrs("    li t2, 1\n");
                    for (int j=0; j<i ;j++) {
                        dumpInstrs("    li t3, %d\n", dims[dim_size-j-1]);
                        dumpInstrs("    mul t2, t2, t3\n");
                    }
                    dumpInstrs("    mul t1, t1, t2\n");
                    dumpInstrs("    add t0, t0, t1\n");
                }
                dumpInstrs("    li t1, -4\n");
                dumpInstrs("    mul t0, t0, t1\n");
                if(entry->getKind() == SymbolEntry::KindEnum::kParameterKind) {
                    dumpInstrs("lw t1, -%d(s0)\n", entry->cg_addr);
                } else {
                    dumpInstrs("    li t1, -%d\n", entry->cg_addr);
                    dumpInstrs("    add t1, t1, s0\n");
                }

                dumpInstrs("    add t0, t1, t0\n");
                dumpPushInteger(0);
            }
        }
    } else {
    // value
        if(!entry->getTypePtr()->isReal()) {
            // integer & boolean & string
            if (entry->getLevel() == 0) {
            // Global
                dumpInstrs("    la t0, %s\n", entry->getNameCString());
                dumpInstrs("    lw t1, 0(t0)\n");
                dumpInstrs("    mv t0, t1\n");
                dumpPushInteger(0);
            } else {
            // Local
                int dim_size = p_variable_ref.getIndices().size();
                int array_dim = entry->getTypePtr()->getDimensions().size();
                if(dim_size == 0 && array_dim == 0) {
                    dumpInstrs("    addi t0, s0, -%d\n", entry->cg_addr);
                    dumpInstrs("    lw t1, 0(t0)\n");
                    dumpInstrs("    mv t0, t1\n");
                    dumpPushInteger(0);
                } else if (dim_size == array_dim) {
                    std::vector<uint64_t> dims = entry->getTypePtr()->getDimensions();
                    dumpPopInteger(0);
                    // get indices from stack
                    for (int i=1; i<dim_size ;i++) {
                        dumpPopInteger(1);
                        dumpInstrs("    li t2, 1\n");
                        for (int j=0; j<i ;j++) {
                            dumpInstrs("    li t3, %d\n", dims[dim_size-j-1]);
                            dumpInstrs("    mul t2, t2, t3\n");
                        }
                        dumpInstrs("    mul t1, t1, t2\n");
                        dumpInstrs("    add t0, t0, t1\n");
                    }
                    dumpInstrs("    li t1, -4\n");
                    dumpInstrs("    mul t0, t0, t1\n");
                    if(entry->getKind() == SymbolEntry::KindEnum::kParameterKind) {
                        dumpInstrs("lw t1, -%d(s0)\n", entry->cg_addr);
                    } else {
                        dumpInstrs("    li t1, -%d\n", entry->cg_addr);
                        dumpInstrs("    add t1, t1, s0\n");
                    }
                    dumpInstrs("    add t0, t1, t0\n");
                    dumpInstrs("    lw t1, 0(t0)\n");
                    dumpInstrs("    mv t0, t1\n");
                    dumpPushInteger(0);

                } else {
                    dumpInstrs("    addi t0, s0, -%d\n", entry->cg_addr); 
                    dumpPushInteger(0);   
                }
            }
        } else {
            // real
            dumpInstrs("    addi t0, s0, -%d\n", entry->cg_addr);
            dumpInstrs("    flw ft0, 0(t0)\n");
            dumpPushReal(0);
        }
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {

    p_assignment.setLValuePushAddr();
    

    p_assignment.visitChildNodes(*this);
    // TODO: semantic checks
    //
    p_assignment.unsetLValuePushAddr();

    auto *lvalue = p_assignment.getLvalue();

    auto *lvalue_type = lvalue->getInferredType();
    if (!lvalue_type) {
        return;
    }

    if (!lvalue_type->isScalar()) {
        logSemanticError(lvalue->getLocation(),
                         "array assignment is not allowed");
        return;
    }

    auto *entry = symbol_manager.lookup(lvalue->getName());
    if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind) {
        logSemanticError(lvalue->getLocation(),
                         "cannot assign to variable '%s' which is a constant",
                         lvalue->getNameCString());
        return;
    }

    if (entry->getKind() == SymbolEntry::KindEnum::kLoopVarKind &&
        context_stack.back() != SemanticContext::kForLoop) {
        logSemanticError(lvalue->getLocation(),
                         "the value of loop variable cannot be modified inside "
                         "the loop body");
        return;
    }

    auto *expr = p_assignment.getExpr();
    auto *expr_type = expr->getInferredType();
    if (!expr_type) {
        return;
    }

    if (!expr_type->isScalar()) {
        logSemanticError(expr->getLocation(),
                         "array assignment is not allowed");
        return;
    }

    if (!lvalue_type->compare(expr_type)) {
        logSemanticError(p_assignment.getLocation(),
                         "assigning to '%s' from incompatible type '%s'",
                         lvalue_type->getPTypeCString(),
                         expr_type->getPTypeCString());
        return;
    }

    // Code Generation
    if ((lvalue_type->isInteger() && expr_type->isInteger())
        || (lvalue_type->isBool() && expr_type->isBool())) {
        dumpPopInteger(0);
        dumpPopInteger(1);
        dumpAssignInteger(0, 1);
    } else if (lvalue_type->isString()) {
        dumpPopInteger(0);
        dumpInstrs("    lui t1, %%hi(%s)\n", lvalue->getNameCString());
        dumpInstrs("    addi t2, t1, %%lo(%s)\n", lvalue->getNameCString());
        dumpAssignInteger(2, 0);        
    } else {
        if (p_assignment.isExprConstant()) {
            dumpInstrs("    lui t0, %%hi(%s)\n", lvalue->getNameCString());
            dumpInstrs("    flw ft0, %%lo(%s)(t0)\n", lvalue->getNameCString());
            dumpPopInteger(0);
            dumpAssignReal(0, 0);
        } else if (lvalue_type->isInteger() && expr_type->isReal()){
            dumpPopReal(0);
            dumpPopInteger(0);
            dumpInstrs("    fcvt.w.s t1, ft0\n");
            dumpAssignInteger(1,0);
        } else {
            dumpPopReal(0);
            dumpPopInteger(0);
            dumpAssignReal(0, 0); 
        }
    }

}

void SemanticAnalyzer::visit(ReadNode &p_read) {

    p_read.setTargetPushAddr();

    p_read.visitChildNodes(*this);

    p_read.unsetTargetPushAddr();

    auto *target_type = p_read.getTarget()->getInferredType();
    if (!target_type) {
        return;
    }

    if (!target_type->isScalar()) {
        logSemanticError(
            p_read.getTarget()->getLocation(),
            "variable reference of read statement must be scalar type");
    }

    auto *entry = symbol_manager.lookup(p_read.getTarget()->getName());
    assert(entry && "Shouldn't read here. This should be catched during the "
                    "visits of child nodes");

    if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind ||
        entry->getKind() == SymbolEntry::KindEnum::kLoopVarKind) {
        logSemanticError(p_read.getTarget()->getLocation(),
                         "variable reference of read statement cannot be a "
                         "constant or loop variable");
    }

    // code generation
    if(target_type->isReal()) {
        //dumpPushInteger(0);
        dumpInstrs("    addi sp, sp, -4\n");
        dumpInstrs("    jal ra, readReal\n");
        dumpInstrs("    addi sp, sp, 4\n");
        dumpPopInteger(0);
        //dumpInstrs("    fsw fa0, -%d(s0)\n", entry->cg_addr);
        dumpInstrs("    fsw fa0, 0(t0)\n");
    } else {
        dumpInstrs("    jal ra, readInt\n");
        dumpPopInteger(0);
        dumpInstrs("    sw a0, 0(t0)\n");
    }
}

static void checkConditionValidation(const ExpressionNode *condition) {
    auto *condition_type = condition->getInferredType();
    if (!condition_type) {
        return;
    }

    if (!condition_type->isBool()) {
        logSemanticError(condition->getLocation(),
                         "the expression of condition must be boolean type");
    }
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    p_if.visitChildNodes(*this);

    checkConditionValidation(p_if.getCondition());
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    p_while.visitChildNodes(*this);

    checkConditionValidation(p_while.getCondition());
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    context_stack.emplace_back(SemanticContext::kForLoop);
    symbol_manager.pushScope();

    p_for.visitChildNodes(*this);

    auto *initial_value_node = p_for.getLowerBoundNode();
    auto *condition_value_node = p_for.getUpperBoundNode();

    if (initial_value_node->getConstantPtr()->integer() >
        condition_value_node->getConstantPtr()->integer()) {
        logSemanticError(p_for.getLocation(),
                         "the lower bound and upper bound of iteration count "
                         "must be in the incremental order");
    }

    symbol_manager.popScope();
    context_stack.pop_back();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    p_return.visitChildNodes(*this);

    auto *expected_return_type = return_type_stack.back();
    if (expected_return_type->isVoid()) {
        logSemanticError(p_return.getLocation(),
                         "program/procedure should not return a value");
        return;
    }

    auto *retval = p_return.getRetval();
    auto *real_return_type = retval->getInferredType();
    if (!real_return_type) {
        return;
    }

    if (!expected_return_type->compare(real_return_type)) {
        logSemanticError(retval->getLocation(),
                         "return '%s' from a function with return type '%s'",
                         real_return_type->getPTypeCString(),
                         expected_return_type->getPTypeCString());
        return;
    }

    // code generation
    if(expected_return_type->isReal()){
        dumpPopRealToFA(0);
    } else {
        dumpPopIntegerToA(0);
    }
}
