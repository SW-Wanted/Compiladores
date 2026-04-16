#include "token.h"
const char *token_name(int type)
{
    switch (type) {
        case TOKEN_IDENTIFIER:     return "IDENTIFIER";
        case TOKEN_INT_LITERAL:    return "INT_LITERAL";
        case TOKEN_FLOAT_LITERAL:  return "FLOAT_LITERAL";
        case TOKEN_CHAR_LITERAL:   return "CHAR_LITERAL";
        case TOKEN_STRING_LITERAL: return "STRING_LITERAL";

        case TOKEN_INT:     return "KW_int";
        case TOKEN_FLOAT:   return "KW_float";
        case TOKEN_CHAR:    return "KW_char";
        case TOKEN_VOID:    return "KW_void";
        case TOKEN_IF:      return "KW_if";
        case TOKEN_ELSE:    return "KW_else";
        case TOKEN_WHILE:   return "KW_while";
        case TOKEN_FOR:     return "KW_for";
        case TOKEN_DO:      return "KW_do";
        case TOKEN_RETURN:  return "KW_return";
        case TOKEN_STRUCT:  return "KW_struct";
        case TOKEN_TYPEDEF: return "KW_typedef";
        case TOKEN_INCLUDE: return "KW_include";
        case TOKEN_DEFINE:  return "KW_define";

        case TOKEN_PLUS:    return "OP_PLUS";
        case TOKEN_MINUS:   return "OP_MINUS";
        case TOKEN_STAR:    return "OP_STAR";
        case TOKEN_SLASH:   return "OP_SLASH";
        case TOKEN_PERCENT: return "OP_PERCENT";

        case TOKEN_EQ:  return "OP_EQ";
        case TOKEN_NEQ: return "OP_NEQ";
        case TOKEN_LT:  return "OP_LT";
        case TOKEN_GT:  return "OP_GT";
        case TOKEN_LEQ: return "OP_LEQ";
        case TOKEN_GEQ: return "OP_GEQ";

        case TOKEN_AND: return "OP_AND";
        case TOKEN_OR:  return "OP_OR";
        case TOKEN_NOT: return "OP_NOT";

        case TOKEN_ASSIGN:       return "OP_ASSIGN";
        case TOKEN_PLUS_ASSIGN:  return "OP_PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "OP_MINUS_ASSIGN";

        case TOKEN_LPAREN:    return "LPAREN";
        case TOKEN_RPAREN:    return "RPAREN";
        case TOKEN_LBRACE:    return "LBRACE";
        case TOKEN_RBRACE:    return "RBRACE";
        case TOKEN_LBRACKET:  return "LBRACKET";
        case TOKEN_RBRACKET:  return "RBRACKET";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA:     return "COMMA";
        case TOKEN_DOT:       return "DOT";
        case TOKEN_COLON:     return "COLON";
        case TOKEN_HASH:      return "HASH";
        case TOKEN_AMP:       return "AMP";

        case TOKEN_EOF:     return "EOF";
        case TOKEN_UNKNOWN: return "UNKNOWN";

        default: return "???";
    }
}
