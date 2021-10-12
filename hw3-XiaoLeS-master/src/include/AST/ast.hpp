#ifndef __AST_H
#define __AST_H

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include "visitor/AstNodeVisitor.hpp"

enum VarType {
  VarType_INTEGER = 0,
  VarType_REAL = 1,
  VarType_STRING = 2,
  VarType_BOOLEAN = 3,
  VarType_VOID = 4
};

struct VarTypePack {
  VarType type;
  std::vector<int> *dims;
};

struct IdPack {
  char* name;
  std::pair<int,int> location;
};

struct Location {
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}

    uint32_t line;
    uint32_t col;
};

union ConstVal {
    long ConstVal_INTEGER;
    double ConstVal_REAL;
    bool ConstVal_BOOLEAN;
    char* ConstVal_STRING;
};

struct ConstValPack {
    VarType type;
    std::pair<int,int> location;
    union ConstVal const_val;
};

class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;

  protected:
    const Location location;
};

#endif
