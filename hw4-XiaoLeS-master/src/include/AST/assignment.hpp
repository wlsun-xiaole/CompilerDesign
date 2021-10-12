#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/VariableReference.hpp"
#include "AST/ast.hpp"
#include "AST/expression.hpp"

#include <memory>
#include <cstring>

class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *p_lvalue, ExpressionNode *p_expr);
    ~AssignmentNode() = default;

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
	bool isAnyDirtyLChild();
	bool isAnyDirtyRChild();
	bool isLChildScalar(int& line, int& col);
	bool isRChildScalar(int& line, int& col);
	void setLChildDirty(int idx);
	const char* getLChildPTypeCString();
	const char* getRChildPTypeCString();

	const char* getLChildNameCString();
  private:
    std::unique_ptr<VariableReferenceNode> lvalue;
    std::unique_ptr<ExpressionNode> expr;
};

#endif
