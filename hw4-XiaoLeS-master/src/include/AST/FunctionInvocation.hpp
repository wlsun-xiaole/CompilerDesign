#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"

#include <memory>
#include <vector>
#include <sstream>

class FunctionInvocationNode : public ExpressionNode {
  public:
    typedef std::vector<std::unique_ptr<ExpressionNode>> Exprs;

    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char *p_name, Exprs *p_arguments);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void accept(AstNodeVisitor &p_visitor) override;
    void visitChildNodes(AstNodeVisitor &p_visitor) override;
    int getArgNum();
    bool checkArgs(const char*args, int &line, int &col, std::string& t1, std::string &t2);
  private:
    const std::string name;
    Exprs arguments;
};

#endif
