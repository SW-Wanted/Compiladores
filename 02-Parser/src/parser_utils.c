#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser_utils.h"
#include "parser_error.h"

void parser_next(Parser *parser)
{
    parser->current = parser->next;
    parser->next = analex(&parser->lexer);
}

char *parser_current_lexeme(Parser *parser)
{
    return strdup(parser->current.lexeme);
}

int parser_match(Parser *parser, int type)
{
    if (parser->current.type == type) {
        parser_next(parser);
        return 1;
    }
    return 0;
}

int parser_is_external_type_name(const char *lexeme)
{
    if (!lexeme || !lexeme[0])
        return 0;
    return lexeme[0] >= 'A' && lexeme[0] <= 'Z';
}

int parser_is_local_declaration_start(Parser *parser)
{
    if (parser->current.type == TOKEN_INT || parser->current.type == TOKEN_FLOAT ||
        parser->current.type == TOKEN_CHAR || parser->current.type == TOKEN_VOID ||
        parser->current.type == TOKEN_STRUCT || parser->current.type == TOKEN_UNION ||
        parser->current.type == TOKEN_DOUBLE || parser->current.type == TOKEN_LONG ||
        parser->current.type == TOKEN_SHORT) {
        return 1;
    }
    if (parser->current.type == TOKEN_IDENTIFIER) {
        const char *lex = parser->current.lexeme;
        if (strcmp(lex, "const") == 0 || strcmp(lex, "static") == 0 ||
            strcmp(lex, "unsigned") == 0 || strcmp(lex, "signed") == 0 ||
            strcmp(lex, "volatile") == 0 || strcmp(lex, "extern") == 0) {
            return 1;
        }
        if (parser->next.type == TOKEN_STAR)
            return 1;
        if (parser->next.type == TOKEN_IDENTIFIER) {
            Symbol *sym = scope_lookup(&parser->scope_table, lex);
            return (sym && sym->kind == SYM_TYPEDEF) || parser_is_external_type_name(lex);
        }
        return 0;
    }
    return 0;
}

int parser_type_specifier_starts_after_lparen(Parser *parser)
{
    if (parser->next.type == TOKEN_INT || parser->next.type == TOKEN_FLOAT ||
        parser->next.type == TOKEN_CHAR || parser->next.type == TOKEN_VOID ||
        parser->next.type == TOKEN_DOUBLE || parser->next.type == TOKEN_LONG ||
        parser->next.type == TOKEN_SHORT || parser->next.type == TOKEN_STRUCT ||
        parser->next.type == TOKEN_UNION) {
        return 1;
    }
    if (parser->next.type == TOKEN_IDENTIFIER) {
        const char *lex = parser->next.lexeme;
        return strcmp(lex, "const") == 0 || strcmp(lex, "static") == 0 ||
               strcmp(lex, "unsigned") == 0 || strcmp(lex, "signed") == 0 ||
               strcmp(lex, "volatile") == 0 || strcmp(lex, "extern") == 0;
    }
    return 0;
}

int parser_is_sync_token(Parser *parser)
{
    return parser->current.type == TOKEN_SEMICOLON ||
           parser->current.type == TOKEN_IF || parser->current.type == TOKEN_WHILE ||
           parser->current.type == TOKEN_FOR || parser->current.type == TOKEN_DO ||
           parser->current.type == TOKEN_RETURN || parser->current.type == TOKEN_LBRACE ||
           parser->current.type == TOKEN_RBRACE || parser->current.type == TOKEN_HASH ||
           parser->current.type == TOKEN_TYPEDEF || parser->current.type == TOKEN_STRUCT ||
           parser->current.type == TOKEN_UNION || parser->current.type == TOKEN_SWITCH ||
           parser->current.type == TOKEN_CASE || parser->current.type == TOKEN_DEFAULT ||
           parser->current.type == TOKEN_BREAK || parser->current.type == TOKEN_CONTINUE ||
           parser->current.type == TOKEN_INT || parser->current.type == TOKEN_FLOAT ||
           parser->current.type == TOKEN_CHAR || parser->current.type == TOKEN_VOID ||
           parser->current.type == TOKEN_DOUBLE || parser->current.type == TOKEN_LONG ||
           parser->current.type == TOKEN_SHORT ||
           parser->current.type == TOKEN_IDENTIFIER || parser->current.type == TOKEN_EOF;
}

int parser_is_global_declaration_start(Parser *parser)
{
    return parser->current.type == TOKEN_HASH || parser->current.type == TOKEN_TYPEDEF ||
           parser->current.type == TOKEN_INT || parser->current.type == TOKEN_FLOAT ||
           parser->current.type == TOKEN_CHAR || parser->current.type == TOKEN_VOID ||
           parser->current.type == TOKEN_DOUBLE || parser->current.type == TOKEN_LONG ||
           parser->current.type == TOKEN_SHORT || parser->current.type == TOKEN_STRUCT ||
           parser->current.type == TOKEN_UNION || parser->current.type == TOKEN_IDENTIFIER;
}

ASTNode *parser_error_at_current(Parser *parser, const char *message)
{
    parser_add_error(parser, message, parser->current.line, parser->current.column);
    return ast_new(AST_ERROR, message, parser->current.line, parser->current.column);
}

ASTNode *parser_error_and_sync_global(Parser *parser, const char *message)
{
    ASTNode *error = parser_error_at_current(parser, message);

    if (parser->current.type != TOKEN_EOF)
        parser_next(parser);
    while (parser->current.type != TOKEN_EOF) {
        if (parser->current.type == TOKEN_SEMICOLON || parser->current.type == TOKEN_RBRACE) {
            parser_next(parser);
            break;
        }
        parser_next(parser);
    }
    return error;
}

void parser_expect_closing_paren(Parser *parser)
{
    if (parser->current.type == TOKEN_RPAREN) {
        parser_next(parser);
        return;
    }

    parser_add_error(parser, "esperado )", parser->current.line, parser->current.column);
    while (parser->current.type != TOKEN_RPAREN && parser->current.type != TOKEN_LBRACE &&
           parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_SEMICOLON &&
           parser->current.type != TOKEN_EOF) {
        parser_next(parser);
    }
    if (parser->current.type == TOKEN_RPAREN)
        parser_next(parser);
}

ASTNode *parser_expect(Parser *parser, int type, ASTNodeKind kind)
{
    if (parser->current.type == type) {
        ASTNode *node = NULL;
        if (kind != AST_ERROR)
            node = ast_new(kind, parser->current.lexeme, parser->current.line, parser->current.column);
        parser_next(parser);
        return node;
    }
    {
        char message[128];
        snprintf(message, sizeof(message), "esperado %s", token_name(type));
        parser_add_error(parser, message, parser->current.line, parser->current.column);
    }
    ASTNode *error = ast_new(AST_ERROR, parser->current.lexeme, parser->current.line, parser->current.column);
    if (parser_is_sync_token(parser))
        return error;
    parser_next(parser);
    return error;
}

ASTNode *parser_expect_semicolon(Parser *parser)
{
    if (parser->current.type == TOKEN_SEMICOLON)
        return parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    if (parser_is_sync_token(parser))
        return parser_error_at_current(parser, "esperado ';'");
    return parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
}
