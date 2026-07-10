#ifndef SEM_SYMTAB_H
#define SEM_SYMTAB_H

#include "sem_types.h"
#include "scope_table.h"

#define SEM_MAX_SYMBOLS 2048
#define SEM_MAX_DEPTH   128
#define SEM_MAX_PARAMS  32

typedef struct {
    char       name[SEM_MAX_NAME];
    SymbolKind kind;
    SemType    type;
    int        scope_level;
    int        line;
    int        column;
    int        param_count;
    SemType    param_types[SEM_MAX_PARAMS];
    int        is_variadic;
    int        is_defined;
    int        is_initialized;
    int        is_used;
    int        active;
} SemSymbol;

typedef struct {
    SemSymbol symbols[SEM_MAX_SYMBOLS];
    int       count;
    int       scope_start[SEM_MAX_DEPTH];
    int       depth;
    int       level;
} SemSymTab;

void       sem_symtab_init(SemSymTab *table);
void       sem_scope_enter(SemSymTab *table);
void       sem_scope_exit(SemSymTab *table);
SemSymbol *sem_declare(SemSymTab *table, const SemSymbol *symbol, int *duplicate);
SemSymbol *sem_lookup(SemSymTab *table, const char *name);
SemSymbol *sem_lookup_current(SemSymTab *table, const char *name);

#endif /* SEM_SYMTAB_H */
