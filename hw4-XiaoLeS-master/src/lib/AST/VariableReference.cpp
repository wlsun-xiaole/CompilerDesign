#include "AST/VariableReference.hpp"
#include "visitor/AstNodeVisitor.hpp"

VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *p_name)
    : ExpressionNode{line, col}, name(p_name) {}

VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *p_name,
                                             Exprs *p_indices)
    : ExpressionNode{line, col}, name(p_name), indices(std::move(*p_indices)) {}

const char *VariableReferenceNode::getNameCString() const {
    return name.c_str();
}

void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &index : indices) {
        index->accept(p_visitor);
    }
}

bool VariableReferenceNode::checkIndexType(uint32_t &rline, uint32_t &rcol) {
    for(auto& index : indices) {
        if(!index->getType()->isInteger()) {
            rline = index->getLocation().line;
            rcol = index->getLocation().col;
            return false;
        }
    }
    return true;
}

int VariableReferenceNode::getDimensionNum() {
    return indices.size();
}

void VariableReferenceNode::updateDirty() {
    for(int i=0; i<6; i++) {
        for(auto& index:indices) {
            dirty[i] = dirty[i] || index->isDirty(i);
        }
    }
}

