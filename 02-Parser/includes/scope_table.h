#ifndef SCOPE_TABLE_H
#define SCOPE_TABLE_H

#include "token.h"

#define MAX_SCOPE_NAME_LEN 256

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
    char        scope_name[MAX_SCOPE_NAME_LEN];
    int         line;
    int         column;
    int         param_count;
    int         active;
} Symbol;

#define MAX_SCOPE_SYMBOLS 1024
#define MAX_SCOPE_DEPTH 64

typedef struct {
    Symbol entries[MAX_SCOPE_SYMBOLS];
    int    count;
    int    current_scope;
    int    scope_depth;
    int    scope_start[MAX_SCOPE_DEPTH];
    char   scope_names[MAX_SCOPE_DEPTH][MAX_SCOPE_NAME_LEN];
} ScopeTable;

void scope_table_init(ScopeTable *table);
void scope_enter(ScopeTable *table);
void scope_enter_named(ScopeTable *table, const char *scope_name);
void scope_exit(ScopeTable *table);
int  scope_insert(ScopeTable *table, const Symbol *symbol);
Symbol *scope_lookup(ScopeTable *table, const char *name);
Symbol *scope_lookup_current(ScopeTable *table, const char *name);
const char *scope_current_name(const ScopeTable *table);
void scope_table_print(const ScopeTable *table);

#endif /* SCOPE_TABLE_H */
