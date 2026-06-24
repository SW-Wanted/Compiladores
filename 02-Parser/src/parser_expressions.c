#include <stdlib.h>

#include "parser_expressions.h"
#include "parser_declarations.h"
#include "parser_utils.h"
#include "parser_error.h"

ASTNode *parse_expressao(Parser *parser)
{
    return parse_atribuicao(parser);
}

ASTNode *parse_atribuicao(Parser *parser)
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

ASTNode *parse_condicional(Parser *parser)
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

ASTNode *parse_logico_ou(Parser *parser)
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

ASTNode *parse_logico_e(Parser *parser)
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

ASTNode *parse_bitwise_ou(Parser *parser)
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

ASTNode *parse_bitwise_xor(Parser *parser)
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

ASTNode *parse_bitwise_e(Parser *parser)
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

ASTNode *parse_igualdade(Parser *parser)
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

ASTNode *parse_relacional(Parser *parser)
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

ASTNode *parse_shift(Parser *parser)
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

ASTNode *parse_aditivo(Parser *parser)
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

ASTNode *parse_multiplicativo(Parser *parser)
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

ASTNode *parse_unario(Parser *parser)
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

ASTNode *parse_pos_fixo(Parser *parser)
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

ASTNode *parse_primario(Parser *parser)
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

ASTNode *parse_argumentos_opcionais(Parser *parser)
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
