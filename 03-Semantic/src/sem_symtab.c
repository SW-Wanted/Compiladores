#include <string.h>

#include "sem_symtab.h"

/**
 * @brief Inicializa a tabela e abre o escopo global (nivel 0).
 */
void sem_symtab_init(SemSymTab *table)
{
    table->count = 0;
    table->depth = 0;
    table->level = 0;
    table->scope_start[0] = 0;
}

/**
 * @brief Abre um novo escopo aninhado.
 */
void sem_scope_enter(SemSymTab *table)
{
    if (table->depth >= SEM_MAX_DEPTH - 1)
        return;
    table->depth++;
    table->level++;
    table->scope_start[table->depth] = table->count;
}

/**
 * @brief Fecha o escopo actual, desactivando os simbolos nele declarados.
 */
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

/**
 * @brief Declara um simbolo no escopo actual.
 *
 * Se ja existir um simbolo activo com o mesmo nome no escopo, sinaliza
 * @p duplicate com 1 e devolve o simbolo existente (nao insere um novo).
 */
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

/**
 * @brief Procura um simbolo activo do escopo mais interno para o mais externo.
 */
SemSymbol *sem_lookup(SemSymTab *table, const char *name)
{
    for (int i = table->count - 1; i >= 0; i--) {
        if (table->symbols[i].active && strcmp(table->symbols[i].name, name) == 0)
            return &table->symbols[i];
    }
    return NULL;
}

/**
 * @brief Procura um simbolo activo apenas no escopo actual.
 */
SemSymbol *sem_lookup_current(SemSymTab *table, const char *name)
{
    int start = table->scope_start[table->depth];
    for (int i = table->count - 1; i >= start; i--) {
        if (table->symbols[i].active && strcmp(table->symbols[i].name, name) == 0)
            return &table->symbols[i];
    }
    return NULL;
}
