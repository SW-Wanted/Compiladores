#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "symbol_table.h"

/* =========================================================
 *  LEXER STATE
 *  Holds all context needed while scanning a source file.
 * ========================================================= */

typedef struct {
    const char *source;   /* Full source code string  */
    int         pos;      /* Current character index  */
    int         line;     /* Current line number      */
} Lexer;

/* --- Lifecycle --- */
void  lexer_init (Lexer *lexer, const char *source);

/* --- Core Functions (as required by the assignment) --- */

/* ler_caractere(): reads the next character, advances position. */
char  ler_caractere   (Lexer *lexer);

/* volta_caractere(): "un-reads" one character (used on "other" transitions). */
void  volta_caractere (Lexer *lexer);

/* analex(): finite-state machine — returns the next Token. */
Token analex          (Lexer *lexer);

/* gravar_token_lexema(): saves the token into the symbol table. */
void  gravar_token_lexema (SymbolTable *table, Token token);

/* --- Top-level: tokenizes entire source, fills the symbol table. --- */
void  lexer_run (Lexer *lexer, SymbolTable *table);

#endif /* LEXER_H */
