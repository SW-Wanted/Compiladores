#ifndef AST_H
#define AST_H

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
    AST_SWITCH_STMT,
    AST_CASE_STMT,
    AST_DEFAULT_STMT,
    AST_BREAK_STMT,
    AST_CONTINUE_STMT,
    AST_RETURN_STMT,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_CALL_EXPR,
    AST_SUBSCRIPT_EXPR,
    AST_MEMBER_EXPR,
    AST_CONDITIONAL_EXPR,
    AST_UNION_DECL,
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

ASTNode *ast_new(ASTNodeKind kind, const char *text, int line, int column);
ASTNode *ast_leaf(ASTNodeKind kind, const char *text);
void     ast_add_child(ASTNode *parent, ASTNode *child);
ASTNode *ast_binary(ASTNodeKind kind, const char *op, ASTNode *left, ASTNode *right);
ASTNode *ast_unary(ASTNodeKind kind, const char *op, ASTNode *operand);
const char *ast_kind_name(ASTNodeKind kind);

#endif
