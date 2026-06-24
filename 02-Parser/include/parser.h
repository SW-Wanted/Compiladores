#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "parser_error.h"
#include "lexer.h"
#include "scope_table.h"

typedef struct Parser {
    Lexer      lexer;
    ScopeTable scope_table;
    Token      current;
    Token      next;
    ASTNode   *root;
    char       pending_scope_name[MAX_SCOPE_NAME_LEN];
    ParserError errors[MAX_PARSER_ERRORS];
    int        error_count;
} Parser;

void parser_init(Parser *parser, const char *source);
int  parser_parse(Parser *parser);
void parser_print_ast(const ASTNode *node, int indent);
void parser_free_ast(ASTNode *node);

#endif
