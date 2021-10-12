#include "AST/if.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SemanticAnalyzer.hpp"

IfNode::IfNode(const uint32_t line, const uint32_t col,
               ExpressionNode *p_condition, CompoundStatementNode *p_body,
               CompoundStatementNode *p_else)
    : AstNode{line, col}, condition(p_condition), body(p_body),
      else_body(p_else) {}

const ExpressionNode *IfNode::getCondition() const { return condition.get(); }

void IfNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    SemanticAnalyzer* analyzer = static_cast<SemanticAnalyzer*>(&p_visitor);
	int label_tmp = analyzer->label_num;
	if (else_body)
		analyzer->label_num += 2;
	else analyzer->label_num++;

    condition->accept(p_visitor);
    analyzer->dumpPopInteger(0);
    analyzer->dumpInstrs("    li t1, 0\n");
    analyzer->dumpInstrs("    beq t0, t1, L%d\n", label_tmp);

    body->accept(p_visitor);
    if (else_body) {
    	analyzer->dumpInstrs("    j L%d\n", label_tmp+1);
    	analyzer->dumpInstrs("L%d:\n", label_tmp);
        else_body->accept(p_visitor);
        analyzer->dumpInstrs("L%d:\n", label_tmp+1);
    } else {
    	analyzer->dumpInstrs("    j L%d\n", label_tmp);
    }
}
