#include <stdio.h>
#include <string.h>
#include "scope_table.h"

static void copy_scope_name(char *destination, const char *source)
{
    if (!source || !source[0]) {
        destination[0] = '\0';
        return;
    }
    strncpy(destination, source, MAX_SCOPE_NAME_LEN - 1);
    destination[MAX_SCOPE_NAME_LEN - 1] = '\0';
}

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
    for (int i = 0; i < MAX_SCOPE_DEPTH; i++) table->scope_offset[i] = 0;
    scope_enter_named(table, "Global");
}

void scope_enter(ScopeTable *table)
{
    scope_enter_named(table, NULL);
}

const char *scope_current_name(const ScopeTable *table)
{
    if (!table || table->scope_depth <= 0)
        return "Global";
    if (table->scope_names[table->scope_depth - 1][0] == '\0')
        return "Global";
    return table->scope_names[table->scope_depth - 1];
}

void scope_enter_named(ScopeTable *table, const char *scope_name)
{
    if (table->scope_depth >= MAX_SCOPE_DEPTH) {
        fprintf(stderr, "Erro: profundidade maxima de escopo excedida\n");
        return;
    }
    const char *parent_scope = scope_current_name(table);
    table->scope_start[table->scope_depth++] = table->count;
    if (scope_name && scope_name[0]) {
        copy_scope_name(table->scope_names[table->scope_depth - 1], scope_name);
    } else {
        copy_scope_name(table->scope_names[table->scope_depth - 1], parent_scope);
    }
    table->current_scope++;
    int new_index = table->scope_depth - 1;
    if (new_index >= 0 && new_index < MAX_SCOPE_DEPTH) {
        int parent_index = new_index - 1;
        if (parent_index >= 0 && parent_index < MAX_SCOPE_DEPTH)
            table->scope_offset[new_index] = table->scope_offset[parent_index];
        else
            table->scope_offset[new_index] = 0;
    }
}

void scope_exit(ScopeTable *table)
{
    if (table->scope_depth <= 1)
        return;

    int start = table->scope_start[table->scope_depth - 1];
    for (int i = start; i < table->count; i++) {
        if (table->entries[i].scope_level == table->current_scope)
            table->entries[i].active = 0;
    }
    table->scope_depth--;
    table->scope_names[table->scope_depth][0] = '\0';
    table->current_scope--;
}

int scope_insert(ScopeTable *table, const Symbol *symbol)
{
    int start = current_scope_start(table);
    for (int i = start; i < table->count; i++) {
        if (strcmp(table->entries[i].name, symbol->name) == 0)
            return -1;
    }
    if (table->count >= MAX_SCOPE_SYMBOLS) {
        fprintf(stderr, "Erro: tabela de escopo cheia\n");
        return -1;
    }
    table->entries[table->count] = *symbol;
    table->entries[table->count].active = 1;
    int depth_index = table->scope_depth - 1;
    if (depth_index < 0) depth_index = 0;
    int size = table->entries[table->count].bytes_size;
    if (size <= 0) size = 4;
    table->entries[table->count].mem_address = table->scope_offset[depth_index];
    table->scope_offset[depth_index] += size;
    return table->count++;
}

Symbol *scope_lookup(ScopeTable *table, const char *name)
{
    for (int i = table->count - 1; i >= 0; i--) {
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0)
            return &table->entries[i];
    }
    return NULL;
}

Symbol *scope_lookup_current(ScopeTable *table, const char *name)
{
    int start = current_scope_start(table);
    for (int i = table->count - 1; i >= start; i--) {
        if (table->entries[i].active && table->entries[i].scope_level == table->current_scope &&
            strcmp(table->entries[i].name, name) == 0)
            return &table->entries[i];
    }
    return NULL;
}

void scope_table_print(const ScopeTable *table)
{
    printf("\n%-5s %-20s %-12s %-18s %-28s %-12s %-10s %-10s %-7s %-7s\n",
        "#", "IDENTIFICADOR", "TIPO", "TIPO_RETORNO", "ESCOPO", "ENDERECO", "TAMANHO", "VALOR", "LINHA", "COLUNA");
    printf("%-5s %-20s %-12s %-18s %-28s %-12s %-10s %-10s %-7s %-7s\n",
        "-----", "--------------------", "------------", "------------------", "----------------------------", "----------", "----------", "----------", "-------", "-------");
    for (int i = 0; i < table->count; i++) {
        const Symbol *sym = &table->entries[i];
        const char *kind = "DESCONHECIDO";
        switch (sym->kind) {
            case SYM_VARIABLE: kind = "VARIAVEL"; break;
            case SYM_FUNCTION: kind = "FUNCAO"; break;
            case SYM_TYPEDEF: kind = "TIPODEF"; break;
            case SYM_PARAM: kind = "PARAMETRO"; break;
            case SYM_ENUM_CONST: kind = "ENUM_CONST"; break;
        }
        printf("%-5d %-20s %-12s %-18s %-28s 0x%-10x %-10d %-10s %-7d %-7d\n",
               i + 1,
               sym->name,
               kind,
               token_name(sym->type_token),
               sym->scope_name[0] ? sym->scope_name : scope_current_name(table),
               sym->mem_address,
               sym->bytes_size,
               sym->assigned_value[0] ? sym->assigned_value : "-",
               sym->line,
               sym->column);
    }
    printf("\n");
}
