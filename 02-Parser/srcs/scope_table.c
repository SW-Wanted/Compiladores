#include <stdio.h>
#include <string.h>
#include "scope_table.h"

static int current_scope_start(const ScopeTable *table)
{
    if (table->scope_depth <= 0)
        return 0;
    return table->scope_start[table->scope_depth - 1];
}

void scope_table_init(ScopeTable *table)
{
    table->count = 0;
    table->current_scope = 0;
    table->scope_depth = 0;
    scope_enter(table);
}

void scope_enter(ScopeTable *table)
{
    if (table->scope_depth >= MAX_SCOPE_DEPTH) {
        fprintf(stderr, "Erro: profundidade maxima de escopo excedida\n");
        return;
    }
    table->scope_start[table->scope_depth++] = table->count;
    table->current_scope++;
}

void scope_exit(ScopeTable *table)
{
    if (table->scope_depth <= 1)
        return;
    table->scope_depth--;
    table->count = table->scope_start[table->scope_depth];
    table->current_scope--;
}

int scope_insert(ScopeTable *table, const Symbol *symbol)
{
    int start = current_scope_start(table);
    for (int i = start; i < table->count; i++) {
        if (strcmp(table->entries[i].name, symbol->name) == 0)
            return 0;
    }
    if (table->count >= MAX_SCOPE_SYMBOLS) {
        fprintf(stderr, "Erro: tabela de escopo cheia\n");
        return -1;
    }
    table->entries[table->count++] = *symbol;
    return 1;
}

Symbol *scope_lookup(ScopeTable *table, const char *name)
{
    for (int i = table->count - 1; i >= 0; i--) {
        if (strcmp(table->entries[i].name, name) == 0)
            return &table->entries[i];
    }
    return NULL;
}

Symbol *scope_lookup_current(ScopeTable *table, const char *name)
{
    int start = current_scope_start(table);
    for (int i = table->count - 1; i >= start; i--) {
        if (strcmp(table->entries[i].name, name) == 0)
            return &table->entries[i];
    }
    return NULL;
}

void scope_table_print(const ScopeTable *table)
{
    printf("\n%-5s %-20s %-10s %-12s %-5s\n", "#", "NOME", "KIND", "TIPO_TOKEN", "ESC");
    printf("%-5s %-20s %-10s %-12s %-5s\n", "-----", "--------------------", "----------", "------------", "-----");
    for (int i = 0; i < table->count; i++) {
        const Symbol *sym = &table->entries[i];
        const char *kind = "UNKNOWN";
        switch (sym->kind) {
            case SYM_VARIABLE: kind = "VARIABLE"; break;
            case SYM_FUNCTION: kind = "FUNCTION"; break;
            case SYM_TYPEDEF: kind = "TYPEDEF"; break;
            case SYM_PARAM: kind = "PARAM"; break;
            case SYM_ENUM_CONST: kind = "ENUM_CONST"; break;
        }
        printf("%-5d %-20s %-10s %-12s %-5d\n",
               i + 1,
               sym->name,
               kind,
               token_name(sym->type_token),
               sym->scope_level);
    }
    printf("\n");
}
