#include "AST/variable.hpp"
#include "visitor/AstNodeVisitor.hpp"

VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const std::string &name,
                           const PTypeSharedPtr &p_type,
                           const std::shared_ptr<ConstantValueNode> &p_constant)
    : AstNode{line, col}, name(std::move(name)), type(p_type),
      constant(p_constant) {}

const char *VariableNode::getNameCString() const { return name.c_str(); }

const char *VariableNode::getTypeCString() const {
    return type->getPTypeCString();
}

PTypeSharedPtr VariableNode::getType() {
  return type;
}

void VariableNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    if (constant) {
        constant->accept(p_visitor);
    }
}


const char *VariableNode::getConstantValueCString() const {
	return this->constant? this->constant->getConstantValueCString():NULL;
}
const char *VariableNode::getKindCString() const {
	return this->constant?"constant":"variable";
}


bool VariableNode::checkType() {
  return type->checkDimensions();
}

void VariableNode::setDirty(int idx) {
  dirty[idx] = true;
}
bool VariableNode::isAnyDirty() {
  bool flag = false;
  for(int i=0; i<6; i++)
    flag = (flag || dirty[i]);
  return flag;
}
