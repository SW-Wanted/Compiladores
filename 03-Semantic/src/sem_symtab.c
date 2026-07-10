#include <string.h>

#include "sem_symtab.h"

void sem_symtab_init(SemSymTab *table)
{
    table->count = 0;
    table->depth = 0;
    table->level = 0;
    table->scope_start[0] = 0;
}

void sem_scope_enter(SemSymTab *table)
{
    if (table->depth >= SEM_MAX_DEPTH - 1)
        return;
    table->depth++;
    table->level++;
    table->scope_start[table->depth] = table->count;
}

void sem_scope_exit(SemSymTab *table)
{
    if (table->depth <= 0)
        return;

    int start = table->scope_start[table->depth];
    for (int i = start; i < table->count; i++)
        table->symbols[i].active = 0;

    table->depth--;
    table->level--;
}

SemSymbol *sem_declare(SemSymTab *table, const SemSymbol *symbol, int *duplicate)
{
    int start = table->scope_start[table->depth];

    for (int i = start; i < table->count; i++) {
        if (table->symbols[i].active && strcmp(table->symbols[i].name, symbol->name) == 0) {
            if (duplicate) *duplicate = 1;
            return &table->symbols[i];
        }
    }

    if (duplicate) *duplicate = 0;
    if (table->count >= SEM_MAX_SYMBOLS)
        return NULL;

    SemSymbol *slot = &table->symbols[table->count++];
    *slot = *symbol;
    slot->scope_level = table->level;
    slot->active = 1;
    return slot;
}

SemSymbol *sem_lookup(SemSymTab *table, const char *name)
{
    for (int i = table->count - 1; i >= 0; i--) {
        if (table->symbols[i].active && strcmp(table->symbols[i].name, name) == 0)
            return &table->symbols[i];
    }
    return NULL;
}

SemSymbol *sem_lookup_current(SemSymTab *table, const char *name)
{
    int start = table->scope_start[table->depth];
    for (int i = table->count - 1; i >= start; i--) {
        if (table->symbols[i].active && strcmp(table->symbols[i].name, name) == 0)
            return &table->symbols[i];
    }
    return NULL;
}
