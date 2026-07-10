#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

ASTNode *ast_new(ASTNodeKind kind, const char *text, int line, int column)
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

ASTNode *ast_leaf(ASTNodeKind kind, const char *text)
{
    return ast_new(kind, text, -1, -1);
}

void ast_add_child(ASTNode *parent, ASTNode *child)
{
    if (!child) return;
    parent->child_count++;
    parent->children = realloc(parent->children, sizeof(ASTNode *) * parent->child_count);
    parent->children[parent->child_count - 1] = child;
}

ASTNode *ast_binary(ASTNodeKind kind, const char *op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = ast_new(kind, op, -1, -1);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}

ASTNode *ast_unary(ASTNodeKind kind, const char *op, ASTNode *operand)
{
    ASTNode *node = ast_new(kind, op, -1, -1);
    ast_add_child(node, operand);
    return node;
}

const char *ast_kind_name(ASTNodeKind kind)
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
        case AST_BREAK_STMT: return "instrucao_break";
        case AST_CONTINUE_STMT: return "instrucao_continue";
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
        printf("\x1b[31m");
        printf("%s", ast_kind_name(node->kind));
        if (node->text)
            printf(": %s", node->text);
        if (node->line >= 0 && node->column >= 0)
            printf(" [linha %d, coluna %d]", node->line, node->column);
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
