#include "AST/decl.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
	std::vector<IdPack> *identifiers, struct VarTypePack *vartype)
    : AstNode{line, col} {
    	//printf("length: %d\n", identifiers->size());
    	for(auto &id : *identifiers) {
    		//printf("%d %d %s\n",id.location.first,id.location.second,id.name);
    		variables.push_back(VariableNode(id.location.first, id.location.second, id.name, vartype));
    	}

    }

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
    std::vector<IdPack>* identifiers, struct ConstValPack* const_value_pack)
    : AstNode{line, col} {
        for(auto &id : *identifiers) {
            //printf("%d %d %s\n",id.location.first,id.location.second,id.name);
            variables.push_back(VariableNode(id.location.first, id.location.second, id.name, const_value_pack));
        }
    }

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {}

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto variable: variables) {
    	variable.accept(p_visitor);
    }
}

void DeclNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

std::string DeclNode::getTypeForPara() const{
    std::string str;
    int cnt = 0;
    for (auto &variable : variables) {
        cnt ++; 
        str += variable.getTypeCString();
        if(cnt != variables.size())
            str += ", ";
    }

    return str;
}