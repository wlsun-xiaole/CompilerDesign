#ifndef __SEMA_SYMBOL_MANAGER_H
#define __SEMA_SYMBOL_MANAGER__H

#include <stack>
#include <vector>
#include <cstdio>
#include <string>
#include "AST/PType.hpp"

class SymbolEntry {
public:
    SymbolEntry(const char*, const char*, int, PTypeSharedPtr, const char*);
    const char* getVariableName() { return var_name; }
    const char* getKind() { return kind; }
    int getLevel() { return level; }
    PTypeSharedPtr getType() { return type; }
    const char* getTypeCString() {return type->getPTypeCString(); }
    const char* getAttr() { return attr == NULL? "" : attr; }
    bool isDirty(int idx) { return group[idx]; }
    void setDirty(int idx) { group[idx] = true; }

private:
    // Variable names
    const char *var_name;
    // Kind
    const char *kind;
    // Level
    int level;
    // type
    PTypeSharedPtr type;
    // attribute
    const char *attr;

    bool group[6] = {false, false, false, false, false, false};
};

class SymbolTable {
public:
    SymbolTable();
    void addSymbol(const char*, const char*, int, PTypeSharedPtr, const char*);
    void dumpSymbol();
    bool findEntry(const char* sym_name);
    bool findLoopVarEntry(const char* sym_name);
    const char* getEntryKind(const char* sym_name);
    void setEntryDirty(const char *sym_name, int idx);
    bool isEntryDirty(const char*sym_name, int idx);
    PTypeSharedPtr getEntryType(const char* sym_name);
    const char* getEntryAttr(const char*);
    int getEntryLevel(const char* sym_name);

    // other methods
private:
    std::vector<SymbolEntry> entries;
};

class SymbolManager {
public:
    SymbolManager();
    void pushScope(SymbolTable *new_scope);
    void popScope();
    SymbolTable* getTop();
    int getCurLevel() { return tables.size()-1; }
    bool findSymbol(const char*, bool isVariable);
    bool findSymbol(const char*);
    const char* getSymbolKind(const char*);
    bool isEntryDirty(const char*, int idx);
    void setEntryDirty(const char*, int idx);
    PTypeSharedPtr getEntryType(const char*);
   // other methods
    std::vector<std::string> loop_vars; 
    int getEntryArgNum(const char* sym_name);
    const char* getEntryArgType(const char* sym_name);
    int getSymbolLevel(const char* sym_name);
    int getLoopInitVal(const char* sym_name);
    
private:
    std::vector <SymbolTable *> tables;
};

#endif