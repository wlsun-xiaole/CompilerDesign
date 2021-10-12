#include "AST/PType.hpp"

const char *kTypeString[] = {"void", "integer", "real", "boolean", "string"};

PType::PType(PrimitiveTypeEnum type) : type(type) {}

void PType::setDimensions(std::vector<uint64_t> &dims) {
    dimensions = std::move(dims);
}

const PType::PrimitiveTypeEnum PType::getPrimitiveType() const { return type; }

// logical constness
const char *PType::getPTypeCString() const {
    if (!type_string_is_valid) {
        type_string += kTypeString[static_cast<int>(type)];

        if (dimensions.size() != 0) {
            type_string += " ";

            for (const auto &dim : dimensions) {
                type_string += "[" + std::to_string(dim) + "]";
            }
        }
        type_string_is_valid = true;
    }

    return type_string.c_str();
}

bool PType::checkDimensions() const {
    for(auto dim: dimensions) {
        if(dim <= 0)
            return false;
    }
    return true;
}

bool PType::isInteger() const {
    if (type == PrimitiveTypeEnum::kIntegerType)
        return true;
    return false;
}

int PType::getDimensionNum() const {
    return dimensions.size();
}
/*
const char *PType::getCheckTypeCString(int dim_diff) const {
    printf("haha: %d\n",dim_diff);
    if (!check_string_is_valid) {
        check_string += kTypeString[static_cast<int>(type)];

        if (dim_diff > 0) {
            check_string += " ";

            for (const auto &dim : dimensions) {
                check_string += "[" + std::to_string(dim) + "]";
                if((--dim_diff) == 0) break;
            }
        }
        check_string_is_valid = true;
    }
    printf("wawa: %d\n",dim_diff);
    return check_string.c_str();
}*/

uint64_t PType::getDimension(int idx) const {
    return dimensions.at(idx);
}