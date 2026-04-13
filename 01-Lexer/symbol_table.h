#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "token.h"

/* =========================================================
 *  SYMBOL TABLE
 *  Stores every token found during lexical analysis.
 * ========================================================= */

#define MAX_SYMBOLS 4096

typedef struct {
    Token  entries[MAX_SYMBOLS];
    int    count;
} SymbolTable;

void  symtable_init    (SymbolTable *table);
void  symtable_insert  (SymbolTable *table, Token token);
void  symtable_print   (const SymbolTable *table);

#endif /* SYMBOL_TABLE_H */
