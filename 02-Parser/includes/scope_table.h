#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H

#include "token.h"

typedef enum {
    SYM_VARIABLE,
    SYM_FUNCTION,
    SYM_TYPEDEF,
    SYM_PARAM,
    SYM_ENUM_CONST,
} SymbolKind;

typedef struct {
    char        name[MAX_LEXEME_LEN];
    SymbolKind  kind;
    int         type_token;
    int         scope_level;
    int         line;
} Symbol;

#define MAX_SCOPE_SYMBOLS 1024
#define MAX_SCOPE_DEPTH 64

typedef struct {
    Symbol entries[MAX_SCOPE_SYMBOLS];
    int    count;
    int    current_scope;
    int    scope_depth;
    int    scope_start[MAX_SCOPE_DEPTH];
} ScopeTable;

void scope_table_init(ScopeTable *table);
void scope_enter(ScopeTable *table);
void scope_exit(ScopeTable *table);
int  scope_insert(ScopeTable *table, const Symbol *symbol);
Symbol *scope_lookup(ScopeTable *table, const char *name);
Symbol *scope_lookup_current(ScopeTable *table, const char *name);
void scope_table_print(const ScopeTable *table);

#endif /* SCOPE_TABLE_H */
