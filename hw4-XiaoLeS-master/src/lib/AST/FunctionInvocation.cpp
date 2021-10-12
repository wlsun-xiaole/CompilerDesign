#include "AST/FunctionInvocation.hpp"
#include "visitor/AstNodeVisitor.hpp"

FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char *p_name,
                                               Exprs *p_arguments)
    : ExpressionNode{line, col}, name(p_name),
      arguments(std::move(*p_arguments)) {}

const char *FunctionInvocationNode::getNameCString() const {
    return name.c_str();
}

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &argument : arguments) {
        argument->accept(p_visitor);
    }
}

int FunctionInvocationNode::getArgNum() {
    return arguments.size();
}

bool FunctionInvocationNode::checkArgs(const char*args, int &line, int &col, std::string& t1, std::string &t2) {
    std::string raw_arg(args);
    std::vector<std::string> arg_decls;
    std::istringstream iss(raw_arg);
    std::string token;
    while (std::getline(iss, token, ','))
    {
        if(token[0] == ' ') {
          token.erase(0, 1);
        }
        arg_decls.push_back(token);
    }

    for(int i=0; i<arguments.size(); i++) {
      std::string a = arg_decls[i];
      std::string b(arguments[i]->getType()->getPTypeCString());
      if(a != b) {
          line = arguments[i]->getLocation().line;
          col = arguments[i]->getLocation().col;
          t1 = b;
          t2 = a;
          return false;
      }
    } 

    return true;
}