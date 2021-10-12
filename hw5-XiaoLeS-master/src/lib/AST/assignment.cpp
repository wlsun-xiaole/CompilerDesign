#include "AST/assignment.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SemanticAnalyzer.hpp"

AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               VariableReferenceNode *p_lvalue,
                               ExpressionNode *p_expr)
    : AstNode{line, col}, lvalue(p_lvalue), expr(p_expr) {}

const VariableReferenceNode *AssignmentNode::getLvalue() const {
    return lvalue.get();
}

const ExpressionNode *AssignmentNode::getExpr() const { return expr.get(); }

void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

bool AssignmentNode::isExprConstant() {
	if(dynamic_cast<ConstantValueNode*>(expr.get()) != NULL) 
		return true;
	return false;
}

void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
	SemanticAnalyzer* analyzer = static_cast<SemanticAnalyzer*>(&p_visitor);
    lvalue->accept(p_visitor);
    if(lvalue->getInferredType()->isString()) {
    	analyzer->openBufferFile();
    	analyzer->dumpInstrs("    .section    .rodata\n");
        analyzer->dumpInstrs("    .align    2\n");
        analyzer->dumpInstrs("%s:\n", lvalue->getNameCString());
    } else if(lvalue->getInferredType()->isReal() && isExprConstant()) {
    	analyzer->openBufferFile();
    	analyzer->dumpInstrs("    .section    .rodata\n");
        analyzer->dumpInstrs("    .align    2\n");
        analyzer->dumpInstrs("%s:\n", lvalue->getNameCString());    	
    }
    expr->accept(p_visitor);
}
