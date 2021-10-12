#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/PType.hpp"
#include "AST/ast.hpp"

class ExpressionNode : public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;
    PTypeSharedPtr getType();
    void setType(PTypeSharedPtr);
    void setDirty(int idx);
    bool isDirty(int idx);
    bool isAnyDirty();

	//void setDimDiff(int diff);
	//int getDimDiff();
  protected:
    // for carrying type of result of an expression
    PTypeSharedPtr type = nullptr;
   // int dim_diff = 0;
    // set dirty
    bool dirty[6] = {false, false, false, false, false, false};

};

#endif
