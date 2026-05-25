#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static ASTNode *parse_programa(Parser *parser);
static ASTNode *parse_lista_declaracoes_globais(Parser *parser);
static ASTNode *parse_declaracao_global(Parser *parser);
static ASTNode *parse_directiva(Parser *parser);
static ASTNode *parse_diretiva_include(Parser *parser);
static ASTNode *parse_diretiva_define(Parser *parser);
static ASTNode *parse_nome_ficheiro(Parser *parser);
static ASTNode *parse_type_specifier(Parser *parser);
static ASTNode *parse_nome_ou_corpo_struct(Parser *parser);
static ASTNode *parse_corpo_campos(Parser *parser);
static ASTNode *parse_campo(Parser *parser);
static ASTNode *parse_lista_declaradores_campo(Parser *parser);
static ASTNode *parse_declaracao_typedef(Parser *parser);
static ASTNode *parse_declaracao_geral(Parser *parser);
static ASTNode *parse_declarator(Parser *parser);
static ASTNode *parse_sufixo_array_opcional(Parser *parser);
static ASTNode *parse_inicializacao_opcional(Parser *parser);
static ASTNode *parse_mais_declaradores(Parser *parser);
static ASTNode *parse_parametros_opcionais(Parser *parser);
static ASTNode *parse_lista_parametros(Parser *parser);
static ASTNode *parse_parametro(Parser *parser);
static ASTNode *parse_resto_parametro(Parser *parser);
static ASTNode *parse_bloco(Parser *parser);
static ASTNode *parse_bloco_interno(Parser *parser);
static ASTNode *parse_item_bloco(Parser *parser);
static ASTNode *parse_declaracao_variavel_local(Parser *parser);
static ASTNode *parse_instrucao(Parser *parser);
static ASTNode *parse_instrucao_expressao(Parser *parser);
static ASTNode *parse_instrucao_if(Parser *parser);
static ASTNode *parse_ramo_else_opcional(Parser *parser);
static ASTNode *parse_instrucao_while(Parser *parser);
static ASTNode *parse_instrucao_for(Parser *parser);
static ASTNode *parse_instrucao_do(Parser *parser);
static ASTNode *parse_instrucao_switch(Parser *parser);
static ASTNode *parse_instrucao_break(Parser *parser);
static ASTNode *parse_instrucao_continue(Parser *parser);
static ASTNode *parse_case_item(Parser *parser);
static ASTNode *parse_expressao_opcional(Parser *parser);
static ASTNode *parse_instrucao_return(Parser *parser);
static ASTNode *parse_expressao(Parser *parser);
static ASTNode *parse_atribuicao(Parser *parser);
static ASTNode *parse_condicional(Parser *parser);
static ASTNode *parse_logico_ou(Parser *parser);
static ASTNode *parser_error(Parser *parser, const char *message);
static ASTNode *parse_logico_e(Parser *parser);
static ASTNode *parse_bitwise_ou(Parser *parser);
static ASTNode *parse_bitwise_xor(Parser *parser);
static ASTNode *parse_bitwise_e(Parser *parser);
static ASTNode *parse_shift(Parser *parser);
static ASTNode *parse_igualdade(Parser *parser);
static ASTNode *parse_relacional(Parser *parser);
static ASTNode *parse_aditivo(Parser *parser);
static ASTNode *parse_multiplicativo(Parser *parser);
static ASTNode *parse_unario(Parser *parser);
static ASTNode *parse_pos_fixo(Parser *parser);
static ASTNode *parse_primario(Parser *parser);
static ASTNode *parse_argumentos_opcionais(Parser *parser);
static const char *declarator_name(const ASTNode *decl);
static void declarator_position(const ASTNode *decl, int *line, int *column);
static int type_specifier_token(const ASTNode *type_spec);
static int insert_symbol(Parser *parser, const ASTNode *type_spec, const ASTNode *decl, SymbolKind kind, int line, int column);
static void insert_declarators(Parser *parser, const ASTNode *type_spec, const ASTNode *node, SymbolKind kind, int line, int column);
static void parser_clear_pending_scope(Parser *parser);
static void parser_set_pending_scope(Parser *parser, const char *scope_name);
static void build_child_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment);
static void parser_add_error(Parser *parser, const char *message, int line, int column);

static ASTNode *ast_new(ASTNodeKind kind, const char *text, int line, int column)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->kind = kind;
    node->text = text ? strdup(text) : NULL;
    node->children = NULL;
    node->child_count = 0;
    node->line = line;
    node->column = column;
    return node;
}

static ASTNode *ast_leaf(ASTNodeKind kind, const char *text)
{
    return ast_new(kind, text, -1, -1);
}

static void ast_add_child(ASTNode *parent, ASTNode *child)
{
    if (!child) return;
    parent->child_count++;
    parent->children = realloc(parent->children, sizeof(ASTNode *) * parent->child_count);
    parent->children[parent->child_count - 1] = child;
}

static ASTNode *ast_binary(ASTNodeKind kind, const char *op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = ast_new(kind, op, -1, -1);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}

static ASTNode *ast_unary(ASTNodeKind kind, const char *op, ASTNode *operand)
{
    ASTNode *node = ast_new(kind, op, -1, -1);
    ast_add_child(node, operand);
    return node;
}

static const char *ast_kind_name(ASTNodeKind kind)
{
    switch (kind) {
        case AST_PROGRAM: return "programa";
        case AST_GLOBAL_DECL: return "declaracao_global";
        case AST_DIRECTIVE_INCLUDE: return "diretiva_include";
        case AST_DIRECTIVE_DEFINE: return "diretiva_define";
        case AST_TYPEDEF_DECL: return "declaracao_typedef";
        case AST_GENERAL_DECL: return "declaracao_geral";
        case AST_TYPE_SPECIFIER: return "tipo";
        case AST_DECLARATOR: return "declarador";
        case AST_POINTER: return "ponteiro";
        case AST_ARRAY: return "array";
        case AST_DECLARATOR_LIST: return "declaradores";
        case AST_PARAM_LIST: return "lista_parametros";
        case AST_PARAM: return "parametro";
        case AST_BLOCK: return "bloco";
        case AST_VAR_DECL: return "declaracao_variavel";
        case AST_FUNC_DECL: return "declaracao_funcao";
        case AST_EXPR_STMT: return "instrucao_expressao";
        case AST_IF_STMT: return "instrucao_if";
        case AST_WHILE_STMT: return "instrucao_while";
        case AST_FOR_STMT: return "instrucao_for";
        case AST_DO_WHILE_STMT: return "instrucao_do";
        case AST_SWITCH_STMT: return "instrucao_switch";
        case AST_CASE_STMT: return "caso";
        case AST_DEFAULT_STMT: return "default";
        case AST_BREAK_STMT: return "break";
        case AST_CONTINUE_STMT: return "continue";
        case AST_RETURN_STMT: return "instrucao_return";
        case AST_BINARY_EXPR: return "expressao_binaria";
        case AST_UNARY_EXPR: return "expressao_unaria";
        case AST_CALL_EXPR: return "chamada_funcao";
        case AST_SUBSCRIPT_EXPR: return "subscrito";
        case AST_MEMBER_EXPR: return "acesso_membro";
        case AST_CONDITIONAL_EXPR: return "expressao_condicional";
        case AST_UNION_DECL: return "declaracao_union";
        case AST_IDENTIFIER: return "identificador";
        case AST_LITERAL: return "literal";
        case AST_INCLUDE_PATH: return "nome_ficheiro";
        case AST_DEFINE_VALUE: return "valor_define";
        case AST_ERROR: return "erro";
        default: return "desconhecido";
    }
}

void parser_print_ast(const ASTNode *node, int indent)
{
    if (!node) return;
    for (int i = 0; i < indent; i++)
        printf("  ");
    if (node->kind == AST_ERROR) {
        /* ANSI red */
        printf("\x1b[31m");
        printf("%s", ast_kind_name(node->kind));
        if (node->text)
            printf(": %s", node->text);
        if (node->line >= 0 && node->column >= 0)
            printf(" [linha %d, coluna %d]", node->line, node->column);
        /* Reset color */
        printf("\x1b[0m");
        printf("\n");
    } else {
        printf("%s", ast_kind_name(node->kind));
        if (node->text)
            printf(": %s", node->text);
        printf("\n");
    }
    for (int i = 0; i < node->child_count; i++)
        parser_print_ast(node->children[i], indent + 1);
}

void parser_free_ast(ASTNode *node)
{
    if (!node) return;
    free(node->text);
    for (int i = 0; i < node->child_count; i++)
        parser_free_ast(node->children[i]);
    free(node->children);
    free(node);
}

static void parser_next(Parser *parser)
{
    parser->current = parser->next;
    parser->next = analex(&parser->lexer);
}

static char *parser_current_lexeme(Parser *parser)
{
    return strdup(parser->current.lexeme);
}

static int parser_match(Parser *parser, int type)
{
    if (parser->current.type == type) {
        parser_next(parser);
        return 1;
    }
    return 0;
}

static int parser_is_local_declaration_start(Parser *parser)
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
            /* If current identifier is a typedef name in scope, treat as declaration */
            Symbol *sym = scope_lookup(&parser->scope_table, lex);
            if (sym && sym->kind == SYM_TYPEDEF)
                return 1;
            return 0;
        }
        return 0;
    }
    return 0;
}

static int parser_type_specifier_starts_after_lparen(Parser *parser)
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

static int parser_is_sync_token(Parser *parser)
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

static ASTNode *parser_expect(Parser *parser, int type, ASTNodeKind kind)
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
    parser_next(parser);
    return error;
}

static ASTNode *parser_expect_semicolon(Parser *parser)
{
    if (parser->current.type == TOKEN_SEMICOLON)
        return parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    if (parser_is_sync_token(parser))
        return parser_error(parser, "esperado ';'");
    return parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
}

static ASTNode *parser_error(Parser *parser, const char *message)
{
    int line = parser->current.line;
    int column = parser->current.column;

    parser_add_error(parser, message, line, column);
    if (parser->current.type != TOKEN_EOF)
        parser_next(parser);
    return ast_new(AST_ERROR, message, line, column);
}

static void parser_add_error(Parser *parser, const char *message, int line, int column)
{
    if (parser->error_count >= MAX_PARSER_ERRORS)
        return;

    ParserError *error = &parser->errors[parser->error_count++];
    strncpy(error->message, message ? message : "erro de sintaxe", sizeof(error->message) - 1);
    error->message[sizeof(error->message) - 1] = '\0';
    error->line = line;
    error->column = column;
}

static const char *declarator_name(const ASTNode *decl)
{
    if (!decl) return NULL;
    if (decl->kind == AST_IDENTIFIER) return decl->text;
    for (int i = 0; i < decl->child_count; i++) {
        const char *name = declarator_name(decl->children[i]);
        if (name) return name;
    }
    return NULL;
}

static void declarator_position(const ASTNode *decl, int *line, int *column)
{
    if (!decl) {
        *line = -1;
        *column = -1;
        return;
    }
    if (decl->kind == AST_IDENTIFIER) {
        *line = decl->line;
        *column = decl->column;
        return;
    }
    for (int i = 0; i < decl->child_count; i++) {
        declarator_position(decl->children[i], line, column);
        if (*line >= 0 && *column >= 0)
            return;
    }
    *line = -1;
    *column = -1;
}

static int type_specifier_token(const ASTNode *type_spec)
{
    if (!type_spec)
        return TOKEN_IDENTIFIER;

    if (type_spec->child_count > 0 && type_spec->text) {
        if (strcmp(type_spec->text, "const") == 0 || strcmp(type_spec->text, "static") == 0 ||
            strcmp(type_spec->text, "unsigned") == 0 || strcmp(type_spec->text, "signed") == 0 ||
            strcmp(type_spec->text, "volatile") == 0 || strcmp(type_spec->text, "extern") == 0) {
            return type_specifier_token(type_spec->children[0]);
        }
    }

    if (!type_spec->text)
        return TOKEN_IDENTIFIER;
    if (strcmp(type_spec->text, "int") == 0) return TOKEN_INT;
    if (strcmp(type_spec->text, "float") == 0) return TOKEN_FLOAT;
    if (strcmp(type_spec->text, "char") == 0) return TOKEN_CHAR;
    if (strcmp(type_spec->text, "void") == 0) return TOKEN_VOID;
    if (strcmp(type_spec->text, "double") == 0) return TOKEN_DOUBLE;
    if (strcmp(type_spec->text, "long") == 0) return TOKEN_LONG;
    if (strcmp(type_spec->text, "short") == 0) return TOKEN_SHORT;
    if (strcmp(type_spec->text, "struct") == 0) return TOKEN_STRUCT;
    if (strcmp(type_spec->text, "union") == 0) return TOKEN_UNION;
    if (type_spec->child_count > 0)
        return type_specifier_token(type_spec->children[0]);
    return TOKEN_IDENTIFIER;
}

static int insert_symbol(Parser *parser, const ASTNode *type_spec, const ASTNode *decl, SymbolKind kind, int line, int column)
{
    const char *name = declarator_name(decl);
    if (!name) return -1;

    Symbol symbol = {0};
    strncpy(symbol.name, name, MAX_LEXEME_LEN - 1);
    symbol.kind = kind;
    symbol.type_token = type_specifier_token(type_spec);
    symbol.scope_level = parser->scope_table.current_scope;
    if (kind == SYM_PARAM && parser->pending_scope_name[0]) {
        strncpy(symbol.scope_name, scope_current_name(&parser->scope_table), MAX_SCOPE_NAME_LEN - 1);
        symbol.scope_name[MAX_SCOPE_NAME_LEN - 1] = '\0';
        size_t used = strlen(symbol.scope_name);
        if (used < MAX_SCOPE_NAME_LEN - 1) {
            strncat(symbol.scope_name, "::", MAX_SCOPE_NAME_LEN - used - 1);
            used = strlen(symbol.scope_name);
            if (used < MAX_SCOPE_NAME_LEN - 1)
                strncat(symbol.scope_name, parser->pending_scope_name, MAX_SCOPE_NAME_LEN - used - 1);
        }
    } else {
        strncpy(symbol.scope_name, scope_current_name(&parser->scope_table), MAX_SCOPE_NAME_LEN - 1);
    }
    symbol.line = line;
    symbol.column = column;
    symbol.param_count = 0;
    return scope_insert(&parser->scope_table, &symbol);
}

static void parser_clear_pending_scope(Parser *parser)
{
    parser->pending_scope_name[0] = '\0';
}

static void parser_set_pending_scope(Parser *parser, const char *scope_name)
{
    if (!scope_name) {
        parser_clear_pending_scope(parser);
        return;
    }
    strncpy(parser->pending_scope_name, scope_name, MAX_SCOPE_NAME_LEN - 1);
    parser->pending_scope_name[MAX_SCOPE_NAME_LEN - 1] = '\0';
}

static void build_child_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment)
{
    if (!parent_scope || !parent_scope[0] || strcmp(parent_scope, "Global") == 0) {
        snprintf(destination, destination_size, "%s", segment);
        return;
    }

    const char *closing = strrchr(parent_scope, ')');
    if (closing && closing > parent_scope) {
        snprintf(destination, destination_size, "%.*s -> %s)", (int)(closing - parent_scope), parent_scope, segment);
        return;
    }

    snprintf(destination, destination_size, "%s (%s)", parent_scope, segment);
}

static void insert_declarators(Parser *parser, const ASTNode *type_spec, const ASTNode *node, SymbolKind kind, int line, int column)
{
    if (!node) return;
    if (node->kind == AST_DECLARATOR) {
        insert_symbol(parser, type_spec, node, kind, line, column);
        return;
    }
    if (node->kind == AST_DECLARATOR_LIST) {
        for (int i = 0; i < node->child_count; i++) {
            if (node->children[i]->kind == AST_DECLARATOR) {
                int child_line = -1, child_col = -1;
                declarator_position(node->children[i], &child_line, &child_col);
                insert_declarators(parser, type_spec, node->children[i], kind, child_line, child_col);
            }
        }
    }
}

static ASTNode *parse_programa(Parser *parser)
{
    ASTNode *node = ast_new(AST_PROGRAM, NULL, -1, -1);
    ast_add_child(node, parse_lista_declaracoes_globais(parser));
    parser_expect(parser, TOKEN_EOF, AST_ERROR);
    return node;
}

static ASTNode *parse_lista_declaracoes_globais(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR_LIST, NULL, -1, -1);
    while (parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_declaracao_global(parser));
    }
    return node;
}

static ASTNode *parse_declaracao_global(Parser *parser)
{
    if (parser->current.type == TOKEN_HASH)
        return parse_directiva(parser);
    if (parser->current.type == TOKEN_TYPEDEF)
        return parse_declaracao_typedef(parser);
    return parse_declaracao_geral(parser);
}

static ASTNode *parse_directiva(Parser *parser)
{
    parser_free_ast(parser_expect(parser, TOKEN_HASH, AST_ERROR));
    if (parser->current.type == TOKEN_INCLUDE)
        return parse_diretiva_include(parser);
    if (parser->current.type == TOKEN_DEFINE)
        return parse_diretiva_define(parser);
    return parser_error(parser, "diretiva desconhecida");
}

static ASTNode *parse_diretiva_include(Parser *parser)
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

static ASTNode *parse_diretiva_define(Parser *parser)
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

static ASTNode *parse_nome_ficheiro(Parser *parser)
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

static ASTNode *parse_declaracao_typedef(Parser *parser)
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

static ASTNode *parse_type_specifier(Parser *parser)
{
    /* Accept common qualifiers (const, static, signed, unsigned, volatile, extern)
       before a base type (e.g., 'static char', 'const int'). */
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
        /* treat certain identifiers as qualifiers that precede a real type */
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

static ASTNode *parse_nome_ou_corpo_struct(Parser *parser)
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

static ASTNode *parse_corpo_campos(Parser *parser)
{
    ASTNode *node = ast_new(AST_BLOCK, NULL, -1, -1);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_campo(parser));
    }
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

static ASTNode *parse_campo(Parser *parser)
{
    ASTNode *node = ast_new(AST_VAR_DECL, NULL, -1, -1);
    ast_add_child(node, parse_type_specifier(parser));
    ast_add_child(node, parse_lista_declaradores_campo(parser));
    parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
    return node;
}

static ASTNode *parse_lista_declaradores_campo(Parser *parser)
{
    ASTNode *node = ast_new(AST_DECLARATOR_LIST, NULL, -1, -1);
    ast_add_child(node, parse_declarator(parser));
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_declarator(parser));
    }
    return node;
}

static ASTNode *parse_declarator(Parser *parser)
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

static ASTNode *parse_sufixo_array_opcional(Parser *parser)
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

static ASTNode *parse_inicializacao_opcional(Parser *parser)
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

static ASTNode *parse_mais_declaradores(Parser *parser)
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

static ASTNode *parse_declaracao_geral(Parser *parser)
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
        /* mark pending scope so parameters can record parent::function as their scope */
        parser_set_pending_scope(parser, declarator_name(decl));
        ASTNode *params = parse_parametros_opcionais(parser);
        /* clear pending scope after parameters processed */
        parser_clear_pending_scope(parser);
        if (func_index >= 0 && params)
            parser->scope_table.entries[func_index].param_count = params->child_count;
        ast_add_child(func, params);
        parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
        if (parser->current.type == TOKEN_SEMICOLON) {
            parser_expect(parser, TOKEN_SEMICOLON, AST_ERROR);
            return func;
        }

        scope_enter_named(&parser->scope_table, declarator_name(decl));
        parser_clear_pending_scope(parser);
        ast_add_child(func, parse_bloco_interno(parser));
        scope_exit(&parser->scope_table);
        return func;
    }

    ASTNode *node = ast_new(AST_VAR_DECL, NULL, -1, -1);
    insert_symbol(parser, type_spec, decl, SYM_VARIABLE, decl_line, decl_col);
    ast_add_child(node, type_spec);
    ast_add_child(node, decl);
    ASTNode *init = parse_inicializacao_opcional(parser);
    if (init) ast_add_child(node, init);
    ASTNode *more = parse_mais_declaradores(parser);
    if (more) {
        insert_declarators(parser, type_spec, more, SYM_VARIABLE, decl_line, decl_col);
        ast_add_child(node, more);
    }
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_parametros_opcionais(Parser *parser)
{
    if (parser->current.type == TOKEN_VOID && parser->next.type == TOKEN_RPAREN) {
        parser_next(parser);
        return ast_new(AST_PARAM_LIST, NULL, -1, -1);
    }
    if (parser->current.type == TOKEN_RPAREN)
        return ast_new(AST_PARAM_LIST, NULL, -1, -1);
    return parse_lista_parametros(parser);
}

static ASTNode *parse_lista_parametros(Parser *parser)
{
    ASTNode *node = ast_new(AST_PARAM_LIST, NULL, -1, -1);
    ast_add_child(node, parse_parametro(parser));
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_parametro(parser));
    }
    return node;
}

static ASTNode *parse_parametro(Parser *parser)
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

static ASTNode *parse_resto_parametro(Parser *parser)
{
    if (parser->current.type == TOKEN_STAR || parser->current.type == TOKEN_IDENTIFIER) {
        return parse_declarator(parser);
    }
    return NULL;
}

static ASTNode *parse_bloco(Parser *parser)
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

static ASTNode *parse_bloco_interno(Parser *parser)
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

static ASTNode *parse_item_bloco(Parser *parser)
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

static ASTNode *parse_declaracao_variavel_local(Parser *parser)
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
    if (init) ast_add_child(node, init);
    ASTNode *more = parse_mais_declaradores(parser);
    if (more) {
        insert_declarators(parser, type_spec, more, SYM_VARIABLE, decl_line, decl_col);
        ast_add_child(node, more);
    }
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_instrucao(Parser *parser)
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

static ASTNode *parse_instrucao_expressao(Parser *parser)
{
    ASTNode *expr = parse_expressao(parser);
    ASTNode *terminator = parser_expect_semicolon(parser);
    /* If semicolon expectation produced an error, prefer returning the error node
       instead of wrapping into an empty expression statement. Free expr to avoid leaks. */
    if (terminator && terminator->kind == AST_ERROR) {
        if (expr) parser_free_ast(expr);
        return terminator;
    }
    /* If there's no expression and no terminator, return NULL (nothing to add).
       This avoids creating empty instrucao_expressao nodes. */
    if (!expr && !terminator) return NULL;
    ASTNode *node = ast_new(AST_EXPR_STMT, NULL, -1, -1);
    ast_add_child(node, expr);
    if (terminator)
        ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_instrucao_if(Parser *parser)
{
    ASTNode *node = ast_new(AST_IF_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_IF, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
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

static ASTNode *parse_ramo_else_opcional(Parser *parser)
{
    if (parser->current.type == TOKEN_ELSE) {
        parser_next(parser);
        ASTNode *node = ast_new(AST_IF_STMT, "else", -1, -1);
        ast_add_child(node, parse_instrucao(parser));
        return node;
    }
    return NULL;
}

static ASTNode *parse_instrucao_while(Parser *parser)
{
    ASTNode *node = ast_new(AST_WHILE_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_WHILE, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
    char scope_name[MAX_SCOPE_NAME_LEN];
    build_child_scope_name(scope_name, sizeof(scope_name), scope_current_name(&parser->scope_table), "while");
    parser_set_pending_scope(parser, scope_name);
    ast_add_child(node, parse_instrucao(parser));
    if (parser->pending_scope_name[0])
        parser_clear_pending_scope(parser);
    return node;
}

static ASTNode *parse_instrucao_for(Parser *parser)
{
    ASTNode *node = ast_new(AST_FOR_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_FOR, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    /* init can be an expression or a local declaration (e.g., for (int i = 0; ...)) */
    if (parser_is_local_declaration_start(parser)) {
        /* parse_declaracao_variavel_local consumes the terminating semicolon */
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

static ASTNode *parse_instrucao_do(Parser *parser)
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

static ASTNode *parse_instrucao_switch(Parser *parser)
{
    ASTNode *node = ast_new(AST_SWITCH_STMT, NULL, -1, -1);
    parser_free_ast(parser_expect(parser, TOKEN_SWITCH, AST_IDENTIFIER));
    parser_expect(parser, TOKEN_LPAREN, AST_ERROR);
    ast_add_child(node, parse_expressao(parser));
    parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
    parser_expect(parser, TOKEN_LBRACE, AST_ERROR);
    while (parser->current.type != TOKEN_RBRACE && parser->current.type != TOKEN_EOF) {
        ast_add_child(node, parse_case_item(parser));
    }
    parser_expect(parser, TOKEN_RBRACE, AST_ERROR);
    return node;
}

static ASTNode *parse_instrucao_break(Parser *parser)
{
    ASTNode *node = ast_new(AST_BREAK_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_BREAK, AST_IDENTIFIER));
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_instrucao_continue(Parser *parser)
{
    ASTNode *node = ast_new(AST_CONTINUE_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_CONTINUE, AST_IDENTIFIER));
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_case_item(Parser *parser)
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

static ASTNode *parse_expressao_opcional(Parser *parser)
{
    if (parser->current.type == TOKEN_SEMICOLON || parser->current.type == TOKEN_RPAREN)
        return NULL;
    return parse_expressao(parser);
}

static ASTNode *parse_instrucao_return(Parser *parser)
{
    ASTNode *node = ast_new(AST_RETURN_STMT, NULL, -1, -1);
    ast_add_child(node, parser_expect(parser, TOKEN_RETURN, AST_IDENTIFIER));
    ASTNode *expr = parse_expressao_opcional(parser);
    if (expr) ast_add_child(node, expr);
    ASTNode *terminator = parser_expect_semicolon(parser);
    if (terminator) ast_add_child(node, terminator);
    return node;
}

static ASTNode *parse_expressao(Parser *parser)
{
    return parse_atribuicao(parser);
}

static ASTNode *parse_atribuicao(Parser *parser)
{
    ASTNode *left = parse_condicional(parser);
    if (parser->current.type == TOKEN_ASSIGN || parser->current.type == TOKEN_PLUS_ASSIGN ||
        parser->current.type == TOKEN_MINUS_ASSIGN || parser->current.type == TOKEN_STAR_ASSIGN ||
        parser->current.type == TOKEN_SLASH_ASSIGN || parser->current.type == TOKEN_PERCENT_ASSIGN ||
        parser->current.type == TOKEN_AMP_ASSIGN || parser->current.type == TOKEN_PIPE_ASSIGN ||
        parser->current.type == TOKEN_CARET_ASSIGN || parser->current.type == TOKEN_LSHIFT_ASSIGN ||
        parser->current.type == TOKEN_RSHIFT_ASSIGN) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_atribuicao(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        return node;
    }
    return left;
}

static ASTNode *parse_condicional(Parser *parser)
{
    ASTNode *condition = parse_logico_ou(parser);
    if (parser->current.type == TOKEN_QUESTION) {
        parser_next(parser);
        ASTNode *when_true = parse_expressao(parser);
        parser_expect(parser, TOKEN_COLON, AST_ERROR);
        ASTNode *when_false = parse_condicional(parser);
        ASTNode *node = ast_new(AST_CONDITIONAL_EXPR, "?:", -1, -1);
        ast_add_child(node, condition);
        ast_add_child(node, when_true);
        ast_add_child(node, when_false);
        return node;
    }
    return condition;
}

static ASTNode *parse_logico_ou(Parser *parser)
{
    ASTNode *left = parse_logico_e(parser);
    while (parser->current.type == TOKEN_OR) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_logico_e(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_logico_e(Parser *parser)
{
    ASTNode *left = parse_bitwise_ou(parser);
    while (parser->current.type == TOKEN_AND) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_bitwise_ou(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_bitwise_ou(Parser *parser)
{
    ASTNode *left = parse_bitwise_xor(parser);
    while (parser->current.type == TOKEN_PIPE) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_bitwise_xor(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_bitwise_xor(Parser *parser)
{
    ASTNode *left = parse_bitwise_e(parser);
    while (parser->current.type == TOKEN_CARET) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_bitwise_e(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_bitwise_e(Parser *parser)
{
    ASTNode *left = parse_igualdade(parser);
    while (parser->current.type == TOKEN_AMP) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_igualdade(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_igualdade(Parser *parser)
{
    ASTNode *left = parse_relacional(parser);
    while (parser->current.type == TOKEN_EQ || parser->current.type == TOKEN_NEQ) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_relacional(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_relacional(Parser *parser)
{
    ASTNode *left = parse_shift(parser);
    while (parser->current.type == TOKEN_LT || parser->current.type == TOKEN_GT ||
           parser->current.type == TOKEN_LEQ || parser->current.type == TOKEN_GEQ) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_shift(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_shift(Parser *parser)
{
    ASTNode *left = parse_aditivo(parser);
    while (parser->current.type == TOKEN_LSHIFT || parser->current.type == TOKEN_RSHIFT) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_aditivo(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_aditivo(Parser *parser)
{
    ASTNode *left = parse_multiplicativo(parser);
    while (parser->current.type == TOKEN_PLUS || parser->current.type == TOKEN_MINUS) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_multiplicativo(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_multiplicativo(Parser *parser)
{
    ASTNode *left = parse_unario(parser);
    while (parser->current.type == TOKEN_STAR || parser->current.type == TOKEN_SLASH || parser->current.type == TOKEN_PERCENT) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *right = parse_unario(parser);
        ASTNode *node = ast_binary(AST_BINARY_EXPR, op, left, right);
        free(op);
        left = node;
    }
    return left;
}

static ASTNode *parse_unario(Parser *parser)
{
    if (parser->current.type == TOKEN_SIZEOF) {
        ASTNode *node = ast_unary(AST_UNARY_EXPR, "sizeof", NULL);
        parser_next(parser);
        if (parser->current.type == TOKEN_LPAREN) {
            int depth = 0;
            do {
                if (parser->current.type == TOKEN_LPAREN) {
                    depth++;
                } else if (parser->current.type == TOKEN_RPAREN) {
                    depth--;
                    if (depth == 0) {
                        parser_next(parser);
                        break;
                    }
                }
                parser_next(parser);
            } while (parser->current.type != TOKEN_EOF && depth > 0);
        } else {
            ast_add_child(node, parse_unario(parser));
        }
        return node;
    }
    if (parser->current.type == TOKEN_NOT || parser->current.type == TOKEN_MINUS ||
        parser->current.type == TOKEN_AMP || parser->current.type == TOKEN_STAR ||
        parser->current.type == TOKEN_PLUS_PLUS || parser->current.type == TOKEN_MINUS_MINUS) {
        char *op = parser_current_lexeme(parser);
        parser_next(parser);
        ASTNode *node = ast_unary(AST_UNARY_EXPR, op, parse_unario(parser));
        free(op);
        return node;
    }
    if (parser->current.type == TOKEN_LPAREN && parser_type_specifier_starts_after_lparen(parser)) {
        parser_next(parser);
        ASTNode *cast_type = parse_type_specifier(parser);
        parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
        ASTNode *node = ast_unary(AST_UNARY_EXPR, "cast", parse_unario(parser));
        ast_add_child(node, cast_type);
        return node;
    }
    return parse_pos_fixo(parser);
}

static ASTNode *parse_pos_fixo(Parser *parser)
{
    ASTNode *node = parse_primario(parser);
    while (1) {
        if (parser->current.type == TOKEN_LBRACKET) {
            parser_next(parser);
            ASTNode *index = parse_expressao(parser);
            parser_expect(parser, TOKEN_RBRACKET, AST_ERROR);
            ASTNode *sub = ast_new(AST_SUBSCRIPT_EXPR, NULL, -1, -1);
            ast_add_child(sub, node);
            ast_add_child(sub, index);
            node = sub;
            continue;
        }
        if (parser->current.type == TOKEN_LPAREN) {
            parser_next(parser);
            ASTNode *call = ast_new(AST_CALL_EXPR, NULL, -1, -1);
            ast_add_child(call, node);
            ast_add_child(call, parse_argumentos_opcionais(parser));
            parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
            node = call;
            continue;
        }
        if (parser->current.type == TOKEN_DOT) {
            parser_next(parser);
            ASTNode *member = ast_new(AST_MEMBER_EXPR, NULL, -1, -1);
            ast_add_child(member, node);
            ast_add_child(member, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
            node = member;
            continue;
        }
        if (parser->current.type == TOKEN_ARROW) {
            parser_next(parser);
            ASTNode *member = ast_new(AST_MEMBER_EXPR, "->", -1, -1);
            ast_add_child(member, node);
            ast_add_child(member, parser_expect(parser, TOKEN_IDENTIFIER, AST_IDENTIFIER));
            node = member;
            continue;
        }
        if (parser->current.type == TOKEN_PLUS_PLUS || parser->current.type == TOKEN_MINUS_MINUS) {
            char *op = parser_current_lexeme(parser);
            parser_next(parser);
            ASTNode *un = ast_unary(AST_UNARY_EXPR, op, node);
            free(op);
            node = un;
            continue;
        }
        break;
    }
    return node;
}

static ASTNode *parse_primario(Parser *parser)
{
    if (parser->current.type == TOKEN_LPAREN) {
        parser_next(parser);
        ASTNode *expr = parse_expressao(parser);
        parser_expect(parser, TOKEN_RPAREN, AST_ERROR);
        return expr;
    }
    if (parser->current.type == TOKEN_IDENTIFIER || parser->current.type == TOKEN_INT_LITERAL ||
        parser->current.type == TOKEN_FLOAT_LITERAL || parser->current.type == TOKEN_CHAR_LITERAL ||
        parser->current.type == TOKEN_STRING_LITERAL) {
        ASTNodeKind kind = parser->current.type == TOKEN_IDENTIFIER ? AST_IDENTIFIER : AST_LITERAL;
        ASTNode *node = ast_leaf(kind, parser->current.lexeme);
        parser_next(parser);
        return node;
    }
    return parser_error(parser, "expressao esperada");
}

static ASTNode *parse_argumentos_opcionais(Parser *parser)
{
    ASTNode *node = ast_new(AST_PARAM_LIST, NULL, -1, -1);
    if (parser->current.type == TOKEN_RPAREN)
        return node;
    ast_add_child(node, parse_expressao(parser));
    while (parser->current.type == TOKEN_COMMA) {
        parser_next(parser);
        ast_add_child(node, parse_expressao(parser));
    }
    return node;
}

int parser_parse(Parser *parser)
{
    parser->current = analex(&parser->lexer);
    parser->next = analex(&parser->lexer);
    parser->root = parse_programa(parser);
    return 0;
}

void parser_init(Parser *parser, const char *source)
{
    lexer_init(&parser->lexer, source);
    scope_table_init(&parser->scope_table);
    parser_clear_pending_scope(parser);
    parser->error_count = 0;
}
