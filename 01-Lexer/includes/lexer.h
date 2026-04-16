#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "symbol_table.h"

/**
 * @brief Estado interno do analisador lexico.
 */

typedef struct {
    const char *source;
    int         pos;
    int         line;
} Lexer;

/**
 * @brief Inicializa o lexer para uma nova entrada.
 * @param lexer Estrutura de estado do lexer.
 * @param source Codigo fonte completo.
 */
void  lexer_init (Lexer *lexer, const char *source);

/**
 * @brief Le o proximo caractere da entrada e avanca o cursor.
 * @param lexer Estrutura de estado do lexer.
 * @return Caractere lido.
 */
char  ler_caractere   (Lexer *lexer);

/**
 * @brief Retrocede um caractere no fluxo de entrada.
 * @param lexer Estrutura de estado do lexer.
 */
void  volta_caractere (Lexer *lexer);

/**
 * @brief Executa o automato lexico e produz o proximo token.
 * @param lexer Estrutura de estado do lexer.
 * @return Proximo token reconhecido.
 */
Token analex          (Lexer *lexer);

/**
 * @brief Registra token reconhecido na tabela de simbolos.
 * @param table Tabela de simbolos de destino.
 * @param token Token a registrar.
 */
void  gravar_token_lexema (SymbolTable *table, Token token);

/**
 * @brief Tokeniza toda a fonte e preenche a tabela de simbolos.
 * @param lexer Estrutura de estado do lexer.
 * @param table Tabela de simbolos de destino.
 */
void  lexer_run (Lexer *lexer, SymbolTable *table);

#endif /* LEXER_H */
