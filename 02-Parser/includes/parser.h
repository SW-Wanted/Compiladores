#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "scope_table.h"

typedef enum {
    AST_PROGRAM,
    AST_GLOBAL_DECL,
    AST_DIRECTIVE_INCLUDE,
    AST_DIRECTIVE_DEFINE,
    AST_TYPEDEF_DECL,
    AST_GENERAL_DECL,
    AST_TYPE_SPECIFIER,
    AST_DECLARATOR,
    AST_POINTER,
    AST_ARRAY,
    AST_DECLARATOR_LIST,
    AST_PARAM_LIST,
    AST_PARAM,
    AST_BLOCK,
    AST_VAR_DECL,
    AST_FUNC_DECL,
    AST_EXPR_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_DO_WHILE_STMT,
    AST_RETURN_STMT,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_CALL_EXPR,
    AST_SUBSCRIPT_EXPR,
    AST_MEMBER_EXPR,
    AST_IDENTIFIER,
    AST_LITERAL,
    AST_INCLUDE_PATH,
    AST_DEFINE_VALUE,
    AST_ERROR
} ASTNodeKind;

typedef struct ASTNode {
    ASTNodeKind kind;
    char      *text;
    struct ASTNode **children;
    int         child_count;
    int         line;
    int         column;
} ASTNode;

typedef struct {
    Lexer      lexer;
    ScopeTable scope_table;
    Token      current;
    Token      next;
    ASTNode   *root;
} Parser;

void parser_init(Parser *parser, const char *source);
int  parser_parse(Parser *parser);
void parser_print_ast(const ASTNode *node, int indent);
void parser_free_ast(ASTNode *node);

#endif
