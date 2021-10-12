#include "AST/while.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SemanticAnalyzer.hpp"

WhileNode::WhileNode(const uint32_t line, const uint32_t col,
                     ExpressionNode *p_condition, CompoundStatementNode *p_body)
    : AstNode{line, col}, condition(p_condition), body(p_body) {}

const ExpressionNode *WhileNode::getCondition() const {
    return condition.get();
}

void WhileNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
	SemanticAnalyzer* analyzer = static_cast<SemanticAnalyzer*>(&p_visitor);
	int label_tmp = analyzer->label_num;
	analyzer->label_num += 2;
   	analyzer->dumpInstrs("L%d:\n", label_tmp);
    condition->accept(p_visitor);
    analyzer->dumpPopInteger(0);
    analyzer->dumpInstrs("    li t1, 0\n");
    analyzer->dumpInstrs("    beq t0, t1, L%d\n", label_tmp+1);
    body->accept(p_visitor);
    analyzer->dumpInstrs("    j L%d\n", label_tmp);
    analyzer->dumpInstrs("L%d:\n", label_tmp+1);
}
