#ifndef __AST_EXPRESSION_NODE_H
#define __AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"

enum OpType {
	OpType_MULTIPLY = 0,
	OpType_DIVIDE,
	OpType_MOD,
	OpType_PLUS,
	OpType_MINUS,
	OpType_LESS,
	OpType_LESS_OR_EQUAL,
	OpType_GREATER,
	OpType_GREATER_OR_EQUAL,
	OpType_EQUAL,
	OpType_NOT_EQUAL,
	OpType_AND,
	OpType_OR,
	OpType_NOT
};

class ExpressionNode : public AstNode {
  public:
    ExpressionNode(const uint32_t line, const uint32_t col);
    ~ExpressionNode() = default;

  protected:
    // for carrying type of result of an expression
    // TODO: for next assignment
};

#endif
