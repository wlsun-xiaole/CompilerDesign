#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
						   char* p_name, std::vector<DeclNode>* p_decls,
						   struct VarTypePack* ret_type_pack,
						   CompoundStatementNode* compound_stat
						  ) : AstNode{line, col}, name(p_name), decls(*p_decls),
							ret_type(*ret_type_pack)
							{
								p_compound_stat = compound_stat;
							}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &decl : decls) {
    	decl.accept(p_visitor);
    }
    if(p_compound_stat != NULL) {
    	p_compound_stat->accept(p_visitor);
    }
}

void FunctionNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

std::string FunctionNode::getNameCString() const { return name; }
std::string FunctionNode::getTypeCString() const {

    std::string str;

    /* Return Type */
    switch(ret_type.type) {
		case VarType_INTEGER:
    		str += "integer";
    		break;
		case VarType_REAL:
			str += "real";
			break;
		case VarType_STRING:
			str += "string";
			break;
		case VarType_BOOLEAN:
			str += "boolean";
			break;
		case VarType_VOID:
			str += "void";
			break;
	}

	str += " (";

	int cnt = 0;
	for (auto &decl : decls) {
		cnt++;
		str += decl.getTypeForPara();
		if (cnt != decls.size()) {
			str += ", ";
		}
	}
	
	str += ")";

	return str;
}