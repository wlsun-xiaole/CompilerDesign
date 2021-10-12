#ifndef __SEMA_CONTEXT_MANAGER_H
#define __SEMA_CONTEXT_MANAGER__H

#include <string>
#include <vector>

class ContextManager {
public:
    ContextManager() {}
    const char* getVarKindCString() {
        switch (this->var_kind) {
            case 1:
                return "variable";
            case 2:
                return "constant";
            case 3:
                return "parameter";
            case 4:
                return "loop_var";
            default:
                return "";
        }
    }
    int getVarKind() { return var_kind; }
    void setVarKind(int kind_idx) {
        var_kind = kind_idx;
    }

    std::vector<std::string> return_types;
    // other methods
private:
    int var_kind;
};

#endif