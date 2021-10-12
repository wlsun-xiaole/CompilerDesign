#include "AST/for.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SemanticAnalyzer.hpp"

#include <cassert>

ForNode::ForNode(const uint32_t line, const uint32_t col, DeclNode *p_var_decl,
                 AssignmentNode *p_initial_statement,
                 ConstantValueNode *p_condition, CompoundStatementNode *p_body)
    : AstNode{line, col}, var_decl(p_var_decl),
      initial_statement(p_initial_statement), condition(p_condition),
      body(p_body) {}

const ConstantValueNode *ForNode::getLowerBoundNode() const {
    auto *initial_value_node =
        dynamic_cast<const ConstantValueNode *>(initial_statement->getExpr());

    assert(initial_value_node && "Shouldn't reach here since the syntax has "
                                 "ensured that it will be a constant value");

    return initial_value_node;
}

const ConstantValueNode *ForNode::getUpperBoundNode() const {
    return condition.get();
}

void ForNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    SemanticAnalyzer* analyzer = static_cast<SemanticAnalyzer*>(&p_visitor);
    int label_tmp = analyzer->label_num;
    analyzer->label_num += 3;

    var_decl->accept(p_visitor);
    initial_statement->accept(p_visitor);

    int addr = initial_statement->getLvalueAddr();

    // push condition to stack
    condition->accept(p_visitor);

    // push current value of loop var to stack
    analyzer->dumpInstrs("L%d:\n", label_tmp);
    analyzer->dumpInstrs("    addi t0, s0, -%d\n", addr);
    analyzer->dumpInstrs("    lw t1, 0(t0)\n");
    analyzer->dumpInstrs("    mv t0, t1\n");
    analyzer->dumpPushInteger(0);

    // check
    analyzer->dumpPopInteger(0); // loop var
    analyzer->dumpPopInteger(1); // condition

    analyzer->dumpInstrs("    blt t0, t1, L%d\n", label_tmp+1);
    analyzer->dumpInstrs("    j L%d\n", label_tmp+2);

    analyzer->dumpInstrs("L%d:\n", label_tmp+1);
    // push back condition for next check
    analyzer->dumpPushInteger(1);

    body->accept(p_visitor);

    // loop var ++
    analyzer->dumpInstrs("    addi t0, s0, -%d\n", addr);
    analyzer->dumpInstrs("    lw t1, 0(t0)\n");
    analyzer->dumpInstrs("    mv t0, t1\n");
    analyzer->dumpInstrs("    li t1, 1\n");
    analyzer->dumpInstrs("    add t0, t0, t1\n");
    analyzer->dumpInstrs("    sw t0, -%d(s0)\n", addr);
    // go check
    analyzer->dumpInstrs("    j L%d\n", label_tmp);
    analyzer->dumpInstrs("L%d:\n", label_tmp+2);
}
