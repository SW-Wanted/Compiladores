#include <string.h>

#include "parser_statements.h"
#include "parser_declarations.h"
#include "parser_expressions.h"
#include "parser_symbols.h"
#include "parser_utils.h"
#include "parser_error.h"

ASTNode *parse_bloco(Parser *parser)
{
    ASTNode *node = ast_new(AST_BLOCK, NULL, -1, -1);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    if (parser->pending_scope_name[0]) {
        scope_enter_named(&parser->scope_table, parser->pending_scope_name);
    } else {
        char fallback_scope[MAX_SCOPE_NAME_LEN];
        build_child_scope_name(fallback_scope, sizeof(fallback_scope), scope_current_name(&parser->scope_table), "bloco");
        scope_enter_named(&parser->scope_table, fallback_scope);
    }
    parser_clear_pending_scope(parser);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ASTNode *child = parse_item_bloco(parser);
        if (child && !(child->kind == AST_EXPR_STMT && child->child_count == 0))
            ast_add_child(node, child);
    }
    scope_exit(&parser->scope_table);
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

ASTNode *parse_bloco_interno(Parser *parser)
{
    ASTNode *node = ast_new(AST_BLOCK, NULL, -1, -1);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ASTNode *child = parse_item_bloco(parser);
        if (child && !(child->kind == AST_EXPR_STMT && child->child_count == 0))
            ast_add_child(node, child);
    }
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

ASTNode *parse_item_bloco(Parser *parser)
{
    if (parser->current.type == TOKEN_SEMICOLON) {
        return parse_instrucao(parser);
    }
    if (parser->current.type == TOKEN_IF || parser->current.type == TOKEN_WHILE ||
        parser->current.type == TOKEN_FOR || parser->current.type == TOKEN_DO ||
        parser->current.type == TOKEN_RETURN || parser->current.type == TOKEN_LBRACE) {
        return parse_instrucao(parser);
    }
    if (parser_is_local_declaration_start(parser)) {
        return parse_declaracao_variavel_local(parser);
    }
    return parse_instrucao(parser);
}

ASTNode *parse_declaracao_variavel_local(Parser *parser)
{
    int decl_line = parser->current.line;
    ASTNode *node = ast_new(AST_VAR_DECL, NULL, -1, -1);
    ASTNode *type_spec = parse_type_specifier(parser);
    ASTNode *decl = parse_declarator(parser);
    int decl_col = -1;
    declarator_position(decl, &decl_line, &decl_col);
    insert_symbol(parser, type_spec, decl, SYM_VARIABLE, decl_line, decl_col);
    ast_add_child(node, type_spec);
    ast_add_child(node, decl);
    ASTNode *init = parse_inicializacao_opcional(parser);
    if (init) {
        ast_add_child(node, init);
        if (init->kind == AST_LITERAL || init->kind == AST_DEFINE_VALUE) {
            const char *name = declarator_name(decl);
            if (name) {
                Symbol *sym = scope_lookup_current(&parser->scope_table, name);
                if (sym && init->text) {
                    strncpy(sym->assigned_value, init->text, sizeof(sym->assigned_value) - 1);
                    sym->assigned_value[sizeof(sym->assigned_value) - 1] = '\0';
                }
            }
        }
    }
    ASTNode *more = parse_mais_declaradores(parser);
    if (more) {
        insert_declarators(parser, type_spec, more, SYM_VARIABLE, decl_line, decl_col);
        ast_add_child(node, more);
    }
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

ASTNode *parse_instrucao(Parser *parser)
{
    if (parser->current.type == TOKEN_IF) return parse_instrucao_if(parser);
    if (parser->current.type == TOKEN_WHILE) return parse_instrucao_while(parser);
    if (parser->current.type == TOKEN_FOR) return parse_instrucao_for(parser);
    if (parser->current.type == TOKEN_DO) return parse_instrucao_do(parser);
    if (parser->current.type == TOKEN_SWITCH) return parse_instrucao_switch(parser);
    if (parser->current.type == TOKEN_BREAK) return parse_instrucao_break(parser);
    if (parser->current.type == TOKEN_CONTINUE) return parse_instrucao_continue(parser);
    if (parser->current.type == TOKEN_RETURN) return parse_instrucao_return(parser);
    if (parser->current.type == TOKEN_LBRACE) return parse_bloco(parser);
    if (parser->current.type == TOKEN_SEMICOLON) {
        parser_next(parser);
        return ast_new(AST_EXPR_STMT, NULL, -1, -1);
    }
    return parse_instrucao_expressao(parser);
}

ASTNode *parse_instrucao_expressao(Parser *parser)
{
    ASTNode *expr = parse_expressao(parser);
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator && terminator->kind == AST_ERROR) {
        if (expr) parser_free_ast(expr);
        return terminator;
    }
    if (!expr && !terminator) return NULL;
    ASTNode *node = ast_new(AST_EXPR_STMT, NULL, -1, -1);
    ast_add_child(node, expr);
    if (terminator)
        ast_add_child(node, terminator);
    return node;
}

ASTNode *parse_instrucao_if(Parser *parser)
{
    ASTNode *node = ast_new(AST_IF_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_IF, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect_closing_paren(parser);
    char scope_name[MAX_SCOPE_NAME_LEN];
    build_child_scope_name(scope_name, sizeof(scope_name), scope_current_name(&parser->scope_table), "if");
    parser_set_pending_scope(parser, scope_name);
    ast_add_child(node, parse_instrucao(parser));
    if (parser->pending_scope_name[0])
        parser_clear_pending_scope(parser);
    ASTNode *else_branch = parse_ramo_else_opcional(parser);
    if (else_branch) ast_add_child(node, else_branch);
    return node;
}

ASTNode *parse_ramo_else_opcional(Parser *parser)
{
    if (parser->current.type == TOKEN_ELSE) {
        parser_next(parser);
        ASTNode *node = ast_new(AST_IF_STMT, "else", -1, -1);
        ast_add_child(node, parse_instrucao(parser));
        return node;
    }
    return NULL;
}

ASTNode *parse_instrucao_while(Parser *parser)
{
    ASTNode *node = ast_new(AST_WHILE_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_WHILE, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect_closing_paren(parser);
    char scope_name[MAX_SCOPE_NAME_LEN];
    build_child_scope_name(scope_name, sizeof(scope_name), scope_current_name(&parser->scope_table), "while");
    parser_set_pending_scope(parser, scope_name);
    ast_add_child(node, parse_instrucao(parser));
    if (parser->pending_scope_name[0])
        parser_clear_pending_scope(parser);
    return node;
}

ASTNode *parse_instrucao_for(Parser *parser)
{
    ASTNode *node = ast_new(AST_FOR_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_FOR, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    if (parser_is_local_declaration_start(parser)) {
        ast_add_child(node, parse_declaracao_variavel_local(parser));
    } else {
        ast_add_child(node, parse_expressao_opcional(parser));
        parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    }

    ast_add_child(node, parse_expressao_opcional(parser));
    parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    ast_add_child(node, parse_expressao_opcional(parser));
    parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
    char scope_name[MAX_SCOPE_NAME_LEN];
    build_child_scope_name(scope_name, sizeof(scope_name), scope_current_name(&parser->scope_table), "for");
    parser_set_pending_scope(parser, scope_name);
    ast_add_child(node, parse_instrucao(parser));
    if (parser->pending_scope_name[0])
        parser_clear_pending_scope(parser);
    return node;
}

ASTNode *parse_instrucao_do(Parser *parser)
{
    ASTNode *node = ast_new(AST_DO_WHILE_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_DO, AST_IDENTIFIER));
    ast_add_child(node, parse_instrucao(parser));
    parser_free_ast(parser_expect(parser, TOKEN_WHILE, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
    parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    return node;
}

ASTNode *parse_instrucao_switch(Parser *parser)
{
    ASTNode *node = ast_new(AST_SWITCH_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_SWITCH, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect_closing_paren(parser);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_case_item(parser));
    }
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

ASTNode *parse_instrucao_break(Parser *parser)
{
    ASTNode *node = ast_new(AST_BREAK_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_BREAK, AST_IDENTIFIER));
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

ASTNode *parse_instrucao_continue(Parser *parser)
{
    ASTNode *node = ast_new(AST_CONTINUE_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_CONTINUE, AST_IDENTIFIER));
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

ASTNode *parse_case_item(Parser *parser)
{
    if (parser->current.type == TOKEN_CASE) {
        ASTNode *node = ast_new(AST_CASE_STMT, NULL, -1, -1);
        parser_expect(parser, TOKEN_CASE, AST_IDENTIFIER);
        ast_add_child(node, parse_expressao(parser));
        parser_expect(parser, TOKEN_COLON, AST_ERROR);
        while (parser->current.type != TOKEN_CASE && parser->current.type != TOKEN_DEFAULT &&
               parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
            ast_add_child(node, parse_item_bloco(parser));
        }
        return node;
    }
    if (parser->current.type == TOKEN_DEFAULT) {
        ASTNode *node = ast_new(AST_DEFAULT_STMT, NULL, -1, -1);
        parser_expect(parser, TOKEN_DEFAULT, AST_IDENTIFIER);
        parser_expect(parser, TOKEN_COLON, AST_ERROR);
        while (parser->current.type != TOKEN_CASE && parser->current.type != TOKEN_DEFAULT &&
               parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
            ast_add_child(node, parse_item_bloco(parser));
        }
        return node;
    }
    return parser_error(parser, "esperado case ou default");
}

ASTNode *parse_expressao_opcional(Parser *parser)
{
    if (parser->current.type == TOKEN_SEMICOLON || parser->current.type == TOKEN_RPAREN)
        return NULL;
    return parse_expressao(parser);
}

ASTNode *parse_instrucao_return(Parser *parser)
{
    ASTNode *node = ast_new(AST_RETURN_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_RETURN, AST_IDENTIFIER));
    ASTNode *expr = parse_expressao_opcional(parser);
    if (expr) ast_add_child(node, expr);
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}
