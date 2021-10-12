#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                 		   const char* p_name, 
                 		   struct VarTypePack *vartype)
    : AstNode{line, col}, name(p_name), var_type(*vartype){
    	/*this->var_type.type = vartype->type;
    	this->var_type.dims = vartype->dims;
*/
    	const_val_node = NULL;
    }
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                 		   const char* p_name,
                 		   struct ConstValPack* const_val_pack)
	: AstNode{line, col}, name(p_name) {
    	/*this->var_type.type = vartype->type;
    	this->var_type.dims = vartype->dims;
*/		
		this->var_type.type = const_val_pack->type;
		this->var_type.dims = NULL;
		const_val_node = new ConstantValueNode(const_val_pack->location.first,
											   const_val_pack->location.second,
											   const_val_pack->const_val,
											   const_val_pack->type);
    }

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if (const_val_node != NULL) {
    	const_val_node->accept(p_visitor);
    }
}
void VariableNode::accept(AstNodeVisitor & p_visitor) {
	p_visitor.visit(*this);
}
std::string VariableNode::getNameCString() const { return name; }
std::string VariableNode::getTypeCString() const {
    std::string str;
    switch(var_type.type) {
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
	}
	if (var_type.dims != NULL) {
		str += " ";
		for (auto& dim : *var_type.dims) {
			str += "[";
			str += std::to_string(dim);
			str += "]";
		}
	}
	//printf("%s\n", str.c_str());
	return str;
}
