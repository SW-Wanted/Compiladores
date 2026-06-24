#include <string.h>

#include "parser_declarations.h"
#include "parser_expressions.h"
#include "parser_statements.h"
#include "parser_symbols.h"
#include "parser_utils.h"
#include "parser_error.h"

ASTNode *parse_programa(Parser *parser)
{
    ASTNode *node = ast_new(AST_PROGRAM, NULL, -1, -1);
    ast_add_child(node, parse_lista_declaracoes_globais(parser));
    parser_expect(parser, TOKEN_EOF, AST_ERROR);
    return node;
}

ASTNode *parse_lista_declaracoes_globais(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR_LIST, NULL, -1, -1);
    while (parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_declaracao_global(parser));
    }
    return node;
}

ASTNode *parse_declaracao_global(Parser *parser)
{
    if (!parser_is_global_declaration_start(parser))
        return parser_error_and_sync_global(parser, "esperado declaracao");
    if (parser->current.type == TOKEN_HASH)
        return parse_directiva(parser);
    if (parser->current.type == TOKEN_TYPEDEF)
        return parse_declaracao_typedef(parser);
    return parse_declaracao_geral(parser);
}

ASTNode *parse_directiva(Parser *parser)
{
    parser_free_ast(parser_expect(parser, TOKEN_HASH, AST_ERROR));
    if (parser->current.type == TOKEN_INCLUDE)
        return parse_diretiva_include(parser);
    if (parser->current.type == TOKEN_DEFINE)
        return parse_diretiva_define(parser);
    return parser_error(parser, "diretiva desconhecida");
}

ASTNode *parse_diretiva_include(Parser *parser)
{
    ASTNode *node = ast_new(AST_DIRECTIVE_INCLUDE, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_INCLUDE, AST_IDENTIFIER));
    if (parser->current.type == TOKEN_LT) {
        parser_expect(parser, TOKEN_LT, AST_ERROR);
        ast_add_child(node, parse_nome_ficheiro(parser));
        parser_expect(parser, TOKEN_GT, AST_ERROR);
    } else if (parser->current.type == TOKEN_STRING_LITERAL) {
        ast_add_child(node, ast_leaf(AST_INCLUDE_PATH, parser->current.lexeme));
        parser_next(parser);
    } else {
        ast_add_child(node, parser_error(parser, "esperado <nome> ou string em include"));
    }
    return node;
}

ASTNode *parse_diretiva_define(Parser *parser)
{
    ASTNode *node = ast_new(AST_DIRECTIVE_DEFINE, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_DEFINE, AST_IDENTIFIER));
    ast_add_child(node, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
    if (parser->current.type == TOKEN_INT_LITERAL || parser->current.type == TOKEN_FLOAT_LITERAL ||
        parser->current.type == TOKEN_STRING_LITERAL || parser->current.type == TOKEN_IDENTIFIER) {
        ast_add_child(node, ast_leaf(AST_DEFINE_VALUE, parser->current.lexeme));
        parser_next(parser);
    }
    return node;
}

ASTNode *parse_nome_ficheiro(Parser *parser)
{
    ASTNode *node = ast_new(AST_INCLUDE_PATH, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
    while (parser->current.type == TOKEN_SLASH || parser->current.type == TOKEN_DOT) {
        ast_add_child(node, ast_leaf(AST_LITERAL, parser->current.lexeme));
        parser_next(parser);
        ast_add_child(node, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
    }
    return node;
}

ASTNode *parse_declaracao_typedef(Parser *parser)
{
    ASTNode *node = ast_new(AST_TYPEDEF_DECL, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_TYPEDEF, AST_IDENTIFIER));
    ASTNode *type_spec = parse_type_specifier(parser);
    ASTNode *decl = parse_declarator(parser);
    int decl_line = -1, decl_col = -1;
    declarator_position(decl, &decl_line, &decl_col);
    insert_symbol(parser, type_spec, decl, SYM_TYPEDEF, decl_line, decl_col);
    ast_add_child(node, type_spec);
    ast_add_child(node, decl);
    parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    return node;
}

ASTNode *parse_type_specifier(Parser *parser)
{
    if (parser->current.type == TOKEN_INT) {
        return parser_expect(parser, TOKEN_INT, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_FLOAT) {
        return parser_expect(parser, TOKEN_FLOAT, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_CHAR) {
        return parser_expect(parser, TOKEN_CHAR, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_VOID) {
        return parser_expect(parser, TOKEN_VOID, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_DOUBLE) {
        return parser_expect(parser, TOKEN_DOUBLE, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_LONG) {
        return parser_expect(parser, TOKEN_LONG, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_SHORT) {
        return parser_expect(parser, TOKEN_SHORT, AST_TYPE_SPECIFIER);
    } else if (parser->current.type == TOKEN_STRUCT) {
        ASTNode *node = parser_expect(parser, TOKEN_STRUCT, AST_TYPE_SPECIFIER);
        ast_add_child(node, parse_nome_ou_corpo_struct(parser));
        return node;
    } else if (parser->current.type == TOKEN_UNION) {
        ASTNode *node = parser_expect(parser, TOKEN_UNION, AST_TYPE_SPECIFIER);
        ast_add_child(node, parse_nome_ou_corpo_struct(parser));
        return node;
    } else if (parser->current.type == TOKEN_IDENTIFIER) {
        const char *lex = parser->current.lexeme;
        if (strcmp(lex, "const") == 0 || strcmp(lex, "static") == 0 ||
            strcmp(lex, "unsigned") == 0 || strcmp(lex, "signed") == 0 ||
            strcmp(lex, "volatile") == 0 || strcmp(lex, "extern") == 0) {
            ASTNode *qual = parser_expect(parser, TOKEN_IDENTIFIER, AST_TYPE_SPECIFIER);
            ast_add_child(qual, parse_type_specifier(parser));
            return qual;
        }
        return parser_expect(parser, TOKEN_IDENTIFIER, AST_TYPE_SPECIFIER);
    }
    return parser_error(parser, "esperado especificador de tipo");
}

ASTNode *parse_nome_ou_corpo_struct(Parser *parser)
{
    if (parser->current.type == TOKEN_IDENTIFIER) {
        ASTNode *node = ast_new(AST_IDENTIFIER, NULL, -1, -1);
        ast_add_child(node, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
        if (parser->current.type == TOKEN_LBRACE)
            ast_add_child(node, parse_corpo_campos(parser));
        return node;
    }
    return parse_corpo_campos(parser);
}

ASTNode *parse_corpo_campos(Parser *parser)
{
    ASTNode *node = ast_new(AST_BLOCK, NULL, -1, -1);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_campo(parser));
    }
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

ASTNode *parse_campo(Parser *parser)
{
    ASTNode *node = ast_new(AST_VAR_DECL, NULL, -1, -1);
    ast_add_child(node, parse_type_specifier(parser));
    ast_add_child(node, parse_lista_declaradores_campo(parser));
    parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    return node;
}

ASTNode *parse_lista_declaradores_campo(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR_LIST, NULL, -1, -1);
    ast_add_child(node, parse_declarator(parser));
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_declarator(parser));
    }
    return node;
}

ASTNode *parse_declarator(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR, NULL, -1, -1);
    while (parser_match(parser, TOKEN_STAR))
        ast_add_child(node, ast_leaf(AST_POINTER, "*"));
    if (parser->current.type == TOKEN_IDENTIFIER)
        ast_add_child(node, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
    else
        ast_add_child(node, parser_error(parser, "esperado identificador"));
    ast_add_child(node, parse_sufixo_array_opcional(parser));
    return node;
}

ASTNode *parse_sufixo_array_opcional(Parser *parser)
{
    ASTNode *node = ast_new(AST_ARRAY, NULL, -1, -1);
    while (parser->current.type == TOKEN_LBRACKET) {
        parser_expect(parser, TOKEN_LBRACKET, AST_ERROR);
        if (parser->current.type != TOKEN_RBRACKET)
            ast_add_child(node, parse_expressao(parser));
        parser_expect(parser, TOKEN_RBRACKET, AST_ERROR);
    }
    if (node->child_count == 0) {
        parser_free_ast(node);
        return NULL;
    }
    return node;
}

ASTNode *parse_inicializacao_opcional(Parser *parser)
{
    if (parser->current.type == TOKEN_ASSIGN) {
        parser_expect(parser, TOKEN_ASSIGN, AST_ERROR);
        if (parser->current.type == TOKEN_LBRACE) {
            ASTNode *node = ast_new(AST_LITERAL, NULL, -1, -1);
            int depth = 0;
            do {
                if (parser->current.type == TOKEN_LBRACE) {
                    depth++;
                } else if (parser->current.type == TOKEN_RBRACE) {
                    depth--;
                    if (depth == 0) {
                        parser_next(parser);
                        break;
                    }
                }
                parser_next(parser);
            } while (parser->current.type != TOKEN_EOF && depth > 0);
            return node;
        }
        return parse_expressao(parser);
    }
    return NULL;
}

ASTNode *parse_mais_declaradores(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR_LIST, NULL, -1, -1);
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_declarator(parser));
        ASTNode *init = parse_inicializacao_opcional(parser);
        if (init)
            ast_add_child(node, init);
    }
    if (node->child_count == 0) {
        parser_free_ast(node);
        return NULL;
    }
    return node;
}

ASTNode *parse_declaracao_geral(Parser *parser)
{
    ASTNode *type_spec = parse_type_specifier(parser);
    if (parser->current.type == TOKEN_SEMICOLON) {
        parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
        ASTNode *decl = ast_new(AST_GENERAL_DECL, NULL, -1, -1);
        ast_add_child(decl, type_spec);
        return decl;
    }

    ASTNode *decl = parse_declarator(parser);
    int decl_line = -1, decl_col = -1;
    declarator_position(decl, &decl_line, &decl_col);
    if (parser->current.type == TOKEN_LPAREN) {
        ASTNode *func = ast_new(AST_FUNC_DECL, NULL, -1, -1);
        int func_index = insert_symbol(parser, type_spec, decl, SYM_FUNCTION, decl_line, decl_col);
        ast_add_child(func, type_spec);
        ast_add_child(func, decl);
        parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
        scope_enter_named(&parser->scope_table, declarator_name(decl));
        parser_clear_pending_scope(parser);
        ASTNode *params = parse_parametros_opcionais(parser);
        if (func_index >= 0 && params)
            parser->scope_table.entries[func_index].param_count = params->child_count;
        ast_add_child(func, params);
        parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
        if (parser->current.type == TOKEN_SEMICOLON) {
            parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
            scope_exit(&parser->scope_table);
            return func;
        }

        ast_add_child(func, parse_bloco_interno(parser));
        scope_exit(&parser->scope_table);
        return func;
    }

    ASTNode *node = ast_new(AST_VAR_DECL, NULL, -1, -1);
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

ASTNode *parse_parametros_opcionais(Parser *parser)
{
    if (parser->current.type == TOKEN_VOID && parser->next.type == TOKEN_RPAREN) {
        parser_next(parser);
        return ast_new(AST_PARAM_LIST, NULL, -1, -1);
    }
    if (parser->current.type == TOKEN_RPAREN)
        return ast_new(AST_PARAM_LIST, NULL, -1, -1);
    return parse_lista_parametros(parser);
}

ASTNode *parse_lista_parametros(Parser *parser)
{
    ASTNode *node = ast_new(AST_PARAM_LIST, NULL, -1, -1);
    ast_add_child(node, parse_parametro(parser));
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_parametro(parser));
    }
    return node;
}

ASTNode *parse_parametro(Parser *parser)
{
    ASTNode *node = ast_new(AST_PARAM, NULL, -1, -1);
    ASTNode *type_spec = parse_type_specifier(parser);
    ASTNode *rest = parse_resto_parametro(parser);
    if (rest) {
        ast_add_child(node, type_spec);
        ast_add_child(node, rest);
        int decl_line = -1, decl_col = -1;
        declarator_position(rest, &decl_line, &decl_col);
        insert_symbol(parser, type_spec, rest, SYM_PARAM, decl_line, decl_col);
    } else {
        ast_add_child(node, type_spec);
    }
    return node;
}

ASTNode *parse_resto_parametro(Parser *parser)
{
    if (parser->current.type == TOKEN_STAR || parser->current.type == TOKEN_IDENTIFIER) {
        return parse_declarator(parser);
    }
    return NULL;
}
