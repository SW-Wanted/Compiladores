#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "parser.h"

void parser_next(Parser *parser);
char *parser_current_lexeme(Parser *parser);
int parser_match(Parser *parser, int type);
int parser_is_external_type_name(const char *lexeme);
int parser_is_local_declaration_start(Parser *parser);
int parser_type_specifier_starts_after_lparen(Parser *parser);
int parser_is_sync_token(Parser *parser);
int parser_is_global_declaration_start(Parser *parser);
ASTNode *parser_error_at_current(Parser *parser, const char *message);
ASTNode *parser_error_and_sync_global(Parser *parser, const char *message);
void parser_expect_closing_paren(Parser *parser);
ASTNode *parser_expect(Parser *parser, int type, ASTNodeKind kind);
ASTNode *parser_expect_semicolon(Parser *parser);

#endif
