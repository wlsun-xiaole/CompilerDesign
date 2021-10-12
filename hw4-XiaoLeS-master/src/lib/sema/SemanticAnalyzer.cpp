#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    //  set opt_table

    // 1. new scope
    SymbolTable *ptable = new SymbolTable();
    this->smanager.pushScope(ptable);
    this->cmanager.setVarKind(0);
    // 2. program node
    cmanager.return_types.push_back(p_program.getTypeCString());
    ptable->addSymbol(p_program.getNameCString(), "program", this->smanager.getCurLevel(), p_program.getType(), NULL);
    // 3. visit child nodes
    p_program.visitChildNodes(*this);
    // 4. semantic analyses

    // 5. pop symbol table
    if(showTable()) ptable->dumpSymbol();
    cmanager.return_types.pop_back();
    this->smanager.popScope();
    delete ptable;
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // 1. skip
    // check redeclaration
    if(this->smanager.findSymbol(p_variable.getNameCString(), true)) {
        // err msg
        hasErr = true;
        printErrRedecl(p_variable.getNameCString(), p_variable.getLocation().line, p_variable.getLocation().col);
        p_variable.setDirty(0);
        return;
    }

    // 2. variable node
    SymbolTable *ptable = this->smanager.getTop();

    if (this->cmanager.getVarKind() == 0) {
        ptable->addSymbol(p_variable.getNameCString(), p_variable.getKindCString(), this->smanager.getCurLevel(), p_variable.getType(), p_variable.getConstantValueCString());
    } else {
        ptable->addSymbol(p_variable.getNameCString(), this->cmanager.getVarKindCString(), this->smanager.getCurLevel(), p_variable.getType(), p_variable.getConstantValueCString());
        if(this->cmanager.getVarKind() == 4) {
            this->smanager.loop_vars.push_back(std::string(p_variable.getNameCString()));
        }
    }

    // 3. vist child nodes
    p_variable.visitChildNodes(*this);
    // 4. check dimension of array > 0
    if(!p_variable.checkType()) {
        // err msg
        hasErr = true;
        printErrVariableDecl(p_variable.getNameCString(), p_variable.getLocation().line, p_variable.getLocation().col);
        ptable->setEntryDirty(p_variable.getNameCString(),0);
        return;
    }
    // 5. skip
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // 1. 2. 3. skip
    // 4.
    p_constant_value.setType(p_constant_value.getTypePtr());
    
    // 5. skip
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // 2. check redecl
    bool eflag = true;
    if (this->smanager.findSymbol(p_function.getNameCString(), false)) {
        eflag = false; 
        // err msg
        hasErr = true;
        printErrRedecl(p_function.getNameCString(), p_function.getLocation().line, p_function.getLocation().col);
    } else {
        this->smanager.getTop()->addSymbol(p_function.getNameCString(), "function", this->smanager.getCurLevel(), p_function.getType(), p_function.getArgsCString());
        cmanager.return_types.push_back(std::string(p_function.getType()->getPTypeCString()));
    }
    // 1. new scope
    SymbolTable *ptable = new SymbolTable();
    this->smanager.pushScope(ptable);
    this->cmanager.setVarKind(3);
    // 3. visit child nodes
    p_function.visitChildNodes(*this);
    this->cmanager.setVarKind(0);
    // 4. semantic analyses
    // 5. pop symbol table
    if(showTable()) ptable->dumpSymbol();
    if(eflag) cmanager.return_types.pop_back();
    this->smanager.popScope();
    delete ptable; 
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    if (this->cmanager.getVarKind() == 3) {
        // don't new scope
        this->cmanager.setVarKind(0);
        p_compound_statement.visitChildNodes(*this);
        this->cmanager.setVarKind(0);

    } else {
        SymbolTable *ptable = new SymbolTable();
        this->smanager.pushScope(ptable);
        this->cmanager.setVarKind(0);

        p_compound_statement.visitChildNodes(*this);
        this->cmanager.setVarKind(0);


        // 5. pop symbol table
        if(showTable()) ptable->dumpSymbol();
        this->smanager.popScope();
        delete ptable; 
    }
    
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // 3.
    p_print.visitChildNodes(*this);

    if(p_print.isAnyDirtyChild()) return;
    int line, col;
    if(!p_print.isChildScalar(line, col)) {
        hasErr = true;
        printErrPrint(line, col);
    }
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // 1. 2. 3.
    p_bin_op.visitChildNodes(*this);

    // inheritant dirty from child nodes
    p_bin_op.updateDirty();

    // dirty check
    if(p_bin_op.isAnyDirty()) return;

    // 4.
    // check group1 & 6
    bool flag_g1 = false;
    bool flag_g3 = false, flag_g4 = false;
    bool flag_g5 = false, flag_g6 = false;
    PTypeSharedPtr RType = p_bin_op.getRightType();
    PTypeSharedPtr LType = p_bin_op.getLeftType();
    //int RDimDiff = p_bin_op.getRightDimDiff();
    //int LDimDiff = p_bin_op.getLeftDimDiff();
    const char* R = RType->getPTypeCString();
    const char* L = LType->getPTypeCString();
    const Operator op = p_bin_op.getOp();

    // Group 1 & 6
    if(op == Operator::kPlusOp || op == Operator::kMinusOp || op == Operator::kMultiplyOp || op == Operator::kDivideOp) {
        flag_g1 = true;
        if(strcmp(L, "integer") == 0) {
            flag_g6 = false;
            if (strcmp(R, "integer") == 0) {
                flag_g1 = false;
                p_bin_op.setType(LType);
            } else if (strcmp(R, "real") == 0) {
                flag_g1 = false;
                p_bin_op.setType(RType);
            } else {
                p_bin_op.setDirty(0);
            }
        } else if (strcmp(L, "real") == 0) {
            flag_g6 = false;
            if (strcmp(R, "integer") == 0 || strcmp(R, "real") == 0) {
                flag_g1 = false;
                p_bin_op.setType(LType);
            } else {
                p_bin_op.setDirty(0);
            }
        } else if (op == Operator::kPlusOp && strcmp(L, "string") == 0) {
            // Group 6
            flag_g6 = true;
            flag_g1 = false;
            if (strcmp(R, "string") == 0) {
                flag_g6 = false;
                p_bin_op.setType(LType);
            } else {
                p_bin_op.setDirty(5);
            }
        } else {
            p_bin_op.setDirty(0);
        }
    } else if (op == Operator::kModOp) { 
        // Group 3
        flag_g3 = true;
        if (strcmp(L, "integer") == 0 && strcmp(R, "integer") == 0) {
            flag_g3 = false;
            p_bin_op.setType(LType);
        } else {
            p_bin_op.setDirty(2);
        }
    } else if (op == Operator::kAndOp || op == Operator::kOrOp) {
        // Group 4
        flag_g4 = true;
        if (strcmp(L, "boolean") == 0 && strcmp(R, "boolean") == 0){
            flag_g4 = false;
            p_bin_op.setType(LType);
        } else {
            p_bin_op.setDirty(3);
        }
    } else if (op == Operator::kLessOp || op == Operator::kLessOrEqualOp || op == Operator::kGreaterOp || op == Operator::kGreaterOrEqualOp || op == Operator::kEqualOp || op == Operator::kNotEqualOp ) {
        // Group 5
        flag_g5 = true;
        if(strcmp(L, "integer") == 0 || strcmp(L, "real") == 0) {
            if (strcmp(R, "integer") == 0 || strcmp(R, "real") == 0) {
                flag_g5 = false;

                // create boolean type
                auto *vref_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
                PTypeSharedPtr s_ptr(vref_type);
                p_bin_op.setType(s_ptr);
            } else {
                p_bin_op.setDirty(4);
            }
        } else {
            p_bin_op.setDirty(4);
        }
    }

        // err msg
    if (flag_g1 || flag_g3 || flag_g4 || flag_g5 || flag_g6) {
        hasErr = true;
        printErrBinOp(p_bin_op.getOpCString(), p_bin_op.getLocation().line, p_bin_op.getLocation().col, R, L);      
    }

}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_un_op.visitChildNodes(*this);

    // inheritant dirty from child nodes
    p_un_op.updateDirty();

    // dirty check
    if(p_un_op.isAnyDirty()) return;


    bool flag_g2 = false, flag_g4 = false;
    PTypeSharedPtr OperandType = p_un_op.getOperandType();
    const char* Operand = OperandType->getPTypeCString();
    const Operator op = p_un_op.getOp();

    if(op == Operator::kNegOp) {
        flag_g2 = true;
        if(strcmp(Operand, "integer") == 0 || strcmp(Operand, "real") == 0) {
            flag_g2 = false;
            p_un_op.setType(OperandType);
        } else {
            p_un_op.setDirty(1);
        }
    } else if (op == Operator::kNotOp) {
        flag_g4 = true;
        if(strcmp(Operand, "boolean") == 0) {
            flag_g4 = false;
            // create boolean type
            auto *vref_type = new PType(PType::PrimitiveTypeEnum::kBoolType);
            PTypeSharedPtr s_ptr(vref_type);
            p_un_op.setType(s_ptr);
        } else {
            p_un_op.setDirty(3);
        }
    } else {
        p_un_op.setDirty(3);
    }

    if (flag_g2 || flag_g4) {
        hasErr = true;
        printErrUnOp(p_un_op.getOpCString(), p_un_op.getLocation().line, p_un_op.getLocation().col, Operand);      
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_func_invocation.visitChildNodes(*this);

    // 4.
        // no decalaration
    if (!smanager.findSymbol(p_func_invocation.getNameCString())) {
        p_func_invocation.setDirty(0);
        hasErr = true;
        printErrFuncInvoc1(p_func_invocation.getNameCString(), p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        return;
    } 

        // not function kind
    const char* kind = smanager.getSymbolKind(p_func_invocation.getNameCString());
    if(kind != NULL) {
        if(strcmp(kind,"function") != 0) {
            p_func_invocation.setDirty(0);
            hasErr = true;
            printErrFuncInvoc2(p_func_invocation.getNameCString(), p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
            return;
        }
    }

        // number of args
    int num_fun_decl = smanager.getEntryArgNum(p_func_invocation.getNameCString());
    int num_fun_invoc = p_func_invocation.getArgNum();
    if (num_fun_decl != num_fun_invoc) {
        p_func_invocation.setDirty(0);
        hasErr = true;
        printErrFuncInvoc3(p_func_invocation.getNameCString(), p_func_invocation.getLocation().line, p_func_invocation.getLocation().col);
        return;
    }

        // traverse args
    const char* args_fun_decl = smanager.getEntryArgType(p_func_invocation.getNameCString());
    int line4 = 0, col4 = 0;
    std::string t1, t2;
    if (!p_func_invocation.checkArgs(args_fun_decl, line4, col4, t1, t2)) {
        p_func_invocation.setDirty(0);
        hasErr = true;
        printErrFuncInvoc4(line4, col4, t1, t2);
    }

    // set type
    PTypeSharedPtr vtype = smanager.getEntryType(p_func_invocation.getNameCString());
    p_func_invocation.setType(vtype);
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    // 3.
    p_variable_ref.visitChildNodes(*this);
    // 4.
        // check in symbol table
    if(!smanager.findSymbol(p_variable_ref.getNameCString())) {
        hasErr = true;
        printErrVariableRef1(p_variable_ref.getNameCString(), p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.setDirty(0);
        return;
    }

        //  dirty check
    if(smanager.isEntryDirty(p_variable_ref.getNameCString(), 0)) {
        p_variable_ref.setDirty(0);
        return;
    }
    p_variable_ref.updateDirty();
    if(p_variable_ref.isAnyDirty()) return;
        // check symbol kind
    const char* kind = smanager.getSymbolKind(p_variable_ref.getNameCString());
    if( kind != NULL) {
        if(strcmp(kind,"parameter") != 0 && strcmp(kind, "variable") != 0 && strcmp(kind, "loop_var") != 0 && strcmp(kind, "constant") != 0) {
            hasErr = true;
            printErrVariableRef2(p_variable_ref.getNameCString(), p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
            p_variable_ref.setDirty(0);
            return;
        }
    } //else { //meet 1 }

    if(smanager.isEntryDirty(p_variable_ref.getNameCString(), 0)) {
        return;
    }
        // check index of array reference
    uint32_t line3, column3;
    if(!p_variable_ref.checkIndexType(line3, column3)) {
        hasErr = true;
        printErrVariableRef3(line3, column3);
        p_variable_ref.setDirty(0);
        return;
    }

    if(smanager.isEntryDirty(p_variable_ref.getNameCString(), 0)) {
        return;
    }
        // check index number
    PTypeSharedPtr vtype = smanager.getEntryType(p_variable_ref.getNameCString());
    int num = vtype->getDimensionNum();
    if(num < p_variable_ref.getDimensionNum()) {
        hasErr = true;
        printErrVariableRef4(p_variable_ref.getNameCString(), p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);
        p_variable_ref.setDirty(0);
        return;        
    } else {
        // set type
        auto *vref_type = new PType(vtype->getPrimitiveType());

        int ed = num;
        int st = p_variable_ref.getDimensionNum();
        std::vector<uint64_t> tmp_dims;
        for(int i=st; i<ed ;i++) {
            tmp_dims.push_back(vtype->getDimension(i));
        }
        vref_type->setDimensions(tmp_dims);
        PTypeSharedPtr s_ptr(vref_type);
        p_variable_ref.setType(s_ptr);
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_assignment.visitChildNodes(*this);

    // LEFT Variable Reference 
    if(p_assignment.isAnyDirtyLChild()) return;

        // not array type
    int Lline, Lcol;
    if(!p_assignment.isLChildScalar(Lline, Lcol)) {
        hasErr = true;
        printErrAssign1(Lline, Lcol);
        return;
    }

    // check kind
    const char* LName = p_assignment.getLChildNameCString();
    const char* kind = smanager.getSymbolKind(LName);

    
    if(kind != NULL) {
        if(strcmp(kind,"constant") == 0) {
            hasErr = true;
            printErrAssign2(Lline, Lcol, LName);
            return;
        } else if (strcmp(kind, "loop_var") == 0){
            int loop_scope = smanager.getSymbolLevel(LName);
            int current_scope = smanager.getCurLevel();
            //printf("Level: %d %d\n", loop_scope, current_scope);
            if(loop_scope < current_scope) {
                hasErr = true;
                printErrAssign3(Lline, Lcol);
                return;
            }
        }
    }


    // RIGHT Expression
    if(p_assignment.isAnyDirtyRChild()) return;
        // not array type
    int Rline, Rcol;
    if(!p_assignment.isRChildScalar(Rline, Rcol)) {
        hasErr = true;
        printErrAssign4(Rline, Rcol);
        return;
    }

    // Both check type
    const char* LType = p_assignment.getLChildPTypeCString();
    const char* RType = p_assignment.getRChildPTypeCString();
    if(strcmp(LType, RType) != 0) {
        if(strcmp(LType, "integer") == 0 || strcmp(LType, "real") == 0) {
            if (strcmp(RType, "integer") == 0 || strcmp(RType, "real") == 0) {
                return;
            }
        }
        // err
        hasErr = true;
        printErrAssign5(p_assignment.getLocation().line, p_assignment.getLocation().col, LType, RType);
    }

    
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_read.visitChildNodes(*this);

    if(p_read.isAnyDirtyChild()) return;

    // check scalar
    int line, col;
    if(!p_read.isChildScalar(line, col)) {
        hasErr = true;
        printErrRead1(line, col);
        return;
    }

    // check kind
    const char* kind = smanager.getSymbolKind(p_read.getChildNameCString());
    if(kind != NULL) {
        if(strcmp(kind,"constant") == 0 || strcmp(kind, "loop_var") == 0) {
            hasErr = true;
            printErrRead2(line, col);
            return;
        }
    }

}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_if.visitChildNodes(*this);

    // check dirty
    if(p_if.isAnyDirtyChild()) return;
    // check condition type
    int line, col;
    PTypeSharedPtr cType = p_if.getChildTypeAndLocation(line, col);
    if(strcmp(cType->getPTypeCString(),"boolean") != 0) {
        hasErr = true;
        printErrIf(line, col);
    }
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_while.visitChildNodes(*this);

    // check dirty
    if(p_while.isAnyDirtyChild()) return;
    // check condition type
    int line, col;
    PTypeSharedPtr cType = p_while.getChildTypeAndLocation(line, col);
    if(strcmp(cType->getPTypeCString(),"boolean") != 0) {
        hasErr = true;
        printErrWhile(line, col);
    }

}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    // 1. new scope
    SymbolTable *ptable = new SymbolTable();
    this->smanager.pushScope(ptable);
    this->cmanager.setVarKind(4);
    // 2. skip
    // 3. visit child nodes
    p_for.visitChildNodes(*this);
    this->cmanager.setVarKind(0);

    // 4. semantic analyses
        // check increasing
    int st = p_for.getStartVal();
    int ed = p_for.getEndVal();
    if(st > ed) {
        hasErr = true;
        printErrFor(p_for.getLocation().line, p_for.getLocation().col);
    }
    
    // 5. pop symbol table
    if(showTable()) ptable->dumpSymbol();
    this->smanager.popScope();
    delete ptable;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    p_return.visitChildNodes(*this);
    const char* ret_type = cmanager.return_types.back().c_str();
    if(strcmp(ret_type, "void") == 0) {
        hasErr = true;
        printErrReturn1(p_return.getLocation().line, p_return.getLocation().col);
        return;
    }
    if(p_return.isAnyDirtyChild()) return;
    int cline, ccol;
    p_return.getChildLocation(cline, ccol);
    const char* expr_type = p_return.getTypeCString();
    if(strcmp(ret_type, expr_type) != 0) {
        hasErr = true;
        printErrReturn2(cline, ccol, ret_type, expr_type);
    }
}

bool SemanticAnalyzer::showTable() {
    if(opt_table == 0)
        return false;
    return true;
}

void SemanticAnalyzer::setOptTable(int t) {
    opt_table = t;
}

void SemanticAnalyzer::printErrRedecl(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: symbol %s is redeclared\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}


void SemanticAnalyzer::printErrVariableDecl(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: '%s' declared as an array with an index that is not greater than 0\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");

}

void SemanticAnalyzer::printErrVariableRef1(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrVariableRef2(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: use of non-variable symbol '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrVariableRef3(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: index of array reference must be an integer\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrVariableRef4(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: there is an over array subscript on '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrBinOp(const char* op, int line, int column, const char* R, const char* L) {
    fprintf(stderr, "<Error> Found in line %d, column %d: invalid operands to binary operator '%s' ('%s' and '%s')\n", line, column, op, L, R);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrUnOp(const char* op, int line, int column, const char* Operand) {
    fprintf(stderr, "<Error> Found in line %d, column %d: invalid operand to unary operator '%s' ('%s')\n", line, column, op, Operand);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrFuncInvoc1(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: use of undeclared symbol '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");    
}

void SemanticAnalyzer::printErrFuncInvoc2(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: call of non-function symbol '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrFuncInvoc3(const char* sym_name, int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: too few/much arguments provided for function '%s'\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrFuncInvoc4(int line, int column, std::string& a, std::string& b) {
    fprintf(stderr, "<Error> Found in line %d, column %d: incompatible type passing '%s' to parameter of type '%s'\n", line, column, a.c_str(), b.c_str());
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrPrint(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: expression of print statement must be scalar type\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrRead1(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement must be scalar type\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
} 

void SemanticAnalyzer::printErrRead2(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: variable reference of read statement cannot be a constant or loop variable\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
} 

void SemanticAnalyzer::printErrAssign1(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: array assignment is not allowed\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrAssign2(int line, int column, const char* sym_name) {
    fprintf(stderr, "<Error> Found in line %d, column %d: cannot assign to variable '%s' which is a constant\n", line, column, sym_name);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrAssign3(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: the value of loop variable cannot be modified inside the loop body\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
} 

void SemanticAnalyzer::printErrAssign4(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: array assignment is not allowed\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
} 

void SemanticAnalyzer::printErrAssign5(int line, int column, const char* t1, const char* t2) {
    fprintf(stderr, "<Error> Found in line %d, column %d: assigning to '%s' from incompatible type '%s'\n", line, column, t1, t2);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrIf(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: the expression of condition must be boolean type\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrWhile(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: the expression of condition must be boolean type\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrFor(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: the lower bound and upper bound of iteration count must be in the incremental order\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}

void SemanticAnalyzer::printErrReturn1(int line, int column) {
    fprintf(stderr, "<Error> Found in line %d, column %d: program/procedure should not return a value\n", line, column);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}


void SemanticAnalyzer::printErrReturn2(int line, int column, const char* ret, const char* expr) {
    fprintf(stderr, "<Error> Found in line %d, column %d: return '%s' from a function with return type '%s'\n", line, column, expr, ret);
    fprintf(stderr, "    %s\n", file_copy[line]);
    for(int i=1; i<column ;i++)
        fprintf(stderr, " ");
    fprintf(stderr, "    ^\n");
}