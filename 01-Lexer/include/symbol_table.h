#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "token.h"

/**
 * @brief Capacidade maxima da tabela de simbolos.
 */
#define MAX_SYMBOLS 4096

/**
 * @brief Estrutura de armazenamento dos tokens reconhecidos.
 */
typedef struct symbolTable {
    Token  entries[MAX_SYMBOLS];
    int    count;
} SymbolTable;

/**
 * @brief Inicializa a tabela de simbolos.
 * @param table Ponteiro para a tabela a inicializar.
 */
void  symtable_init    (SymbolTable *table);

/**
 * @brief Insere um token na tabela de simbolos.
 * @param table Ponteiro para a tabela de simbolos.
 * @param token Token a ser inserido.
 */
void  symtable_insert  (SymbolTable *table, Token token);

/**
 * @brief Imprime o conteudo da tabela de simbolos.
 * @param table Ponteiro constante para a tabela de simbolos.
 */
void  symtable_print   (const SymbolTable *table);

#endif /* SYMBOL_TABLE_H */
