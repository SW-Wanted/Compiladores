#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"

/* =========================================================
 *  SYMBOL TABLE IMPLEMENTATION
 * ========================================================= */

void symtable_init(SymbolTable *table)
{
    table->count = 0;
}

void symtable_insert(SymbolTable *table, Token token)
{
    if (table->count >= MAX_SYMBOLS) {
        fprintf(stderr, "Symbol table overflow!\n");
        exit(1);
    }
    table->entries[table->count++] = token;
}

/* Prints the symbol table in a formatted table. */
void symtable_print(const SymbolTable *table)
{
    printf("\n");
    printf("%-5s  %-25s  %-20s  %s\n",
           "#", "LEXEMA", "TOKEN", "LINHA");
    printf("%-5s  %-25s  %-20s  %s\n",
           "-----", "-------------------------",
           "--------------------", "-----");

    for (int i = 0; i < table->count; i++) {
        const Token *t = &table->entries[i];
        printf("%-5d  %-25s  %-20s  %d\n",
               i + 1,
               t->lexeme,
               token_name(t->type),
               t->line);
    }

    printf("\nTotal de tokens: %d\n\n", table->count);
}
