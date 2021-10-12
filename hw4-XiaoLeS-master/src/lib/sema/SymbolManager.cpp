#include "sema/SymbolManager.hpp"
//#include "visitor/AstNodeInclude.hpp"
//#include "AST/PType.hpp"
#include <cstring>

// SymbolEntry
SymbolEntry::SymbolEntry(const char* vn, const char* k, int lev, PTypeSharedPtr pt, const char* a) {
	this->var_name = vn;
	this->kind = k;
	this->level = lev;
	this->type = pt;
	this->attr = a;
}

// SymbolTable
SymbolTable::SymbolTable() {}
void SymbolTable::addSymbol(const char* vn, const char* k, int lev, PTypeSharedPtr pt, const char* a) {
	this->entries.push_back(SymbolEntry(vn, k, lev, pt, a));
}

void dumpDemarcation(const char chr) {
	for (size_t i = 0; i < 110; ++i) {
		printf("%c", chr);
	}
	puts("");
}

void SymbolTable::dumpSymbol(void) {

	dumpDemarcation('=');
  	printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
                                        "Attribute");
  	dumpDemarcation('-');
  	for (auto &entry : this->entries) {
    	printf("%-33s", entry.getVariableName());
    	printf("%-11s", entry.getKind());
    	printf("%d%-10s", entry.getLevel(), entry.getLevel()?"(local)":"(global)");
    	printf("%-17s", entry.getTypeCString());
    	printf("%-11s", entry.getAttr());
    	puts("");
  	}
	dumpDemarcation('-');
}

bool SymbolTable::findEntry(const char* sym_name) {
	for (auto& entry : entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return true;
		} 
	}
	return false;
}

bool SymbolTable::findLoopVarEntry(const char* sym_name) {
	for (auto& entry : entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0 && strcmp("loop_var", entry.getKind()) == 0) {
			return true;
		} 
	}
	return false;
}

const char* SymbolTable::getEntryKind(const char* sym_name) {
	for (auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return entry.getKind();
		}
	}
	return NULL;
}

void SymbolTable::setEntryDirty(const char* sym_name, int idx) {
	for (auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			entry.setDirty(idx);
			return;
		}
	}
}

bool SymbolTable::isEntryDirty(const char*sym_name, int idx) {
	for (auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return entry.isDirty(idx);
		}
	}
}

PTypeSharedPtr SymbolTable::getEntryType(const char* sym_name) {
	for(auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return entry.getType();
		}
	}
}

const char* SymbolTable::getEntryAttr(const char* sym_name) {
	for(auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return entry.getAttr();
		}
	}
}

int SymbolTable::getEntryLevel(const char* sym_name) {
	for(auto& entry: entries) {
		if(strcmp(sym_name, entry.getVariableName()) == 0) {
			return entry.getLevel();
		}
	}
}

// SymbolManager
SymbolManager::SymbolManager() {}
void SymbolManager::pushScope(SymbolTable *new_scope) {
	tables.push_back(new_scope);
}
void SymbolManager::popScope() {
	// find loop_var
	int count = 0;
	for(auto loop_var: loop_vars) {
		if(tables.back()->findLoopVarEntry(loop_var.c_str())) {
			loop_vars.erase(loop_vars.begin()+count, loop_vars.end());
			break;
		}
		count++;
	}
	tables.pop_back();
}
SymbolTable* SymbolManager::getTop() {
	return tables.back();
}

bool SymbolManager::findSymbol(const char* sym_name, bool isVariable) {
	if (isVariable) {
		// Variable Node
		if(tables.back()->findEntry(sym_name)) {
			return true;
		}

		for(auto& loop_var : loop_vars) {
			if(strcmp(loop_var.c_str(),sym_name)==0)
				return true;
		}
		return false;
	} else {
		// Function Node
		// check current scope
		if(tables.back()->findEntry(sym_name)) {
			return true;
		} 
		return false;
	}
}

bool SymbolManager::findSymbol(const char*sym_name) {
	for (auto& table : tables) {
		if (table->findEntry(sym_name))
			return true;
	}
	return false;
}

const char* SymbolManager::getSymbolKind(const char* sym_name) {
	for(auto it = tables.rbegin(); it!=tables.rend(); it++) {
		const char* res = (*it)->getEntryKind(sym_name);
		if(res != NULL)
			return res; 
	}
	return NULL;
}

void SymbolManager::setEntryDirty(const char* sym_name, int idx) {
	for (auto it = tables.rbegin(); it!=tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			(*it)->setEntryDirty(sym_name, idx);
			return;
		}
	}
}

bool SymbolManager::isEntryDirty(const char* sym_name, int idx) {
	for (auto it = tables.rbegin(); it!=tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			return (*it)->isEntryDirty(sym_name, idx);
		}
	}

	// never
	return true;
}

PTypeSharedPtr SymbolManager::getEntryType(const char* sym_name) {
	for(auto it = tables.rbegin(); it != tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			return (*it)->getEntryType(sym_name);
		}
	}

}

int SymbolManager::getEntryArgNum(const char* sym_name) {
	const char* args;
	for(auto it = tables.rbegin(); it != tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			args = (*it)->getEntryAttr(sym_name);
		}
	}
	int count = 0;
	int len = strlen(args);

	if(len == 0) count = 0;
	else {
		count = 1;
		for(int i=0; i<strlen(args); i++) {
			if(args[i] == ',')
				count++;
		}
	}
	return count;
}

const char* SymbolManager::getEntryArgType(const char* sym_name) {
	for(auto it = tables.rbegin(); it != tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			return (*it)->getEntryAttr(sym_name);
		}
	}
}

int SymbolManager::getSymbolLevel(const char* sym_name) {
	for(auto it = tables.rbegin(); it != tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			return (*it)->getEntryLevel(sym_name);
		}
	}
}

int SymbolManager::getLoopInitVal(const char* sym_name) {
	for(auto it = tables.rbegin(); it != tables.rend(); it++) {
		if((*it)->findEntry(sym_name)) {
			return atoi((*it)->getEntryAttr(sym_name));
		}
	}
}
