#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col
	, union ConstVal const_val, VarType type_v)
    : ExpressionNode{line, col}, value(const_val), type(type_v){}

ConstantValueNode::ConstantValueNode(ConstValPack* p_pack)
	: ExpressionNode{p_pack->location.first,p_pack->location.second},
	  value(p_pack->const_val), type(p_pack->type) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

void ConstantValueNode::accept(AstNodeVisitor &p_visitor) {
	p_visitor.visit(*this);
}

std::string ConstantValueNode::getValueCString() const {
	std::string str;
	switch(this->type) {
		case VarType_INTEGER:
    		str += std::to_string(this->value.ConstVal_INTEGER);
    		break;
		case VarType_REAL:
			str += std::to_string(this->value.ConstVal_REAL);
			break;
		case VarType_STRING:
			str += std::string(this->value.ConstVal_STRING);
			break;
		case VarType_BOOLEAN:
			str += this->value.ConstVal_BOOLEAN ? "true" : "false";
			break;
	}

	return str;
}
