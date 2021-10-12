#include "AST/read.hpp"
#include "visitor/AstNodeVisitor.hpp"

ReadNode::ReadNode(const uint32_t line, const uint32_t col,
                   VariableReferenceNode *p_target)
    : AstNode{line, col}, target(p_target) {}

void ReadNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    target->accept(p_visitor);
}

bool ReadNode::isAnyDirtyChild() {
	return target->isAnyDirty();
}

bool ReadNode::isChildScalar(int& line, int& col) {
	const char* tar = target->getType()->getPTypeCString();
	line = target->getLocation().line;
	col = target->getLocation().col;
	if(strcmp(tar, "integer") == 0 || strcmp(tar, "real") == 0 || strcmp(tar, "string") == 0 || strcmp(tar, "boolean") == 0) {
		return true;
	}
	return false;
}

const char* ReadNode::getChildNameCString() {
	return target->getNameCString();
}
