#ifndef __AST_PTYPE_H
#define __AST_PTYPE_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class PType;

typedef std::shared_ptr<const PType> PTypeSharedPtr;

class PType {
  public:
    enum class PrimitiveTypeEnum : uint8_t {
        kVoidType,
        kIntegerType,
        kRealType,
        kBoolType,
        kStringType
    };

    PType(PrimitiveTypeEnum type);
    ~PType() = default;

    void setDimensions(std::vector<uint64_t> &dims);
    int getDimensionNum() const;
    const PrimitiveTypeEnum getPrimitiveType() const;
    const char *getPTypeCString() const;
    //const char *getCheckTypeCString(int dim_diff) const;

    // check dimensions
    bool checkDimensions() const;
    bool isInteger() const;

    uint64_t getDimension(int idx) const;
  private:
    PrimitiveTypeEnum type;
    std::vector<uint64_t> dimensions;
    mutable std::string type_string;
    mutable bool type_string_is_valid = false;
//    mutable std::string check_string;
//    mutable bool check_string_is_valid = false;
};

#endif
