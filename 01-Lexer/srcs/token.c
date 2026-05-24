#include "token.h"
const char *token_name(int type)
{
    switch (type) {
        case TOKEN_IDENTIFIER:     return "TOKEN_IDENTIFIER";
        case TOKEN_INT_LITERAL:    return "TOKEN_INT_LITERAL";
        case TOKEN_FLOAT_LITERAL:  return "TOKEN_FLOAT_LITERAL";
        case TOKEN_CHAR_LITERAL:   return "TOKEN_CHAR_LITERAL";
        case TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";

        case TOKEN_INT:     return "TOKEN_INT";
        case TOKEN_FLOAT:   return "TOKEN_FLOAT";
        case TOKEN_CHAR:    return "TOKEN_CHAR";
        case TOKEN_VOID:    return "TOKEN_VOID";
        case TOKEN_IF:      return "TOKEN_IF";
        case TOKEN_ELSE:    return "TOKEN_ELSE";
        case TOKEN_WHILE:   return "TOKEN_WHILE";
        case TOKEN_FOR:     return "TOKEN_FOR";
        case TOKEN_DO:      return "TOKEN_DO";
        case TOKEN_RETURN:  return "TOKEN_RETURN";
        case TOKEN_STRUCT:  return "TOKEN_STRUCT";
        case TOKEN_TYPEDEF: return "TOKEN_TYPEDEF";
        case TOKEN_INCLUDE: return "TOKEN_INCLUDE";
        case TOKEN_DEFINE:  return "TOKEN_DEFINE";

        case TOKEN_PLUS:    return "TOKEN_PLUS";
        case TOKEN_MINUS:   return "TOKEN_MINUS";
        case TOKEN_STAR:    return "TOKEN_STAR";
        case TOKEN_SLASH:   return "TOKEN_SLASH";
        case TOKEN_PERCENT: return "TOKEN_PERCENT";
        case TOKEN_PLUS_PLUS: return "TOKEN_PLUS_PLUS";
        case TOKEN_MINUS_MINUS: return "TOKEN_MINUS_MINUS";

        case TOKEN_EQ:  return "TOKEN_EQ";
        case TOKEN_NEQ: return "TOKEN_NEQ";
        case TOKEN_LT:  return "TOKEN_LT";
        case TOKEN_GT:  return "TOKEN_GT";
        case TOKEN_LEQ: return "TOKEN_LEQ";
        case TOKEN_GEQ: return "TOKEN_GEQ";

        case TOKEN_AND: return "TOKEN_AND";
        case TOKEN_OR:  return "TOKEN_OR";
        case TOKEN_NOT: return "TOKEN_NOT";

        case TOKEN_ASSIGN:       return "TOKEN_ASSIGN";
        case TOKEN_PLUS_ASSIGN:  return "TOKEN_PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "TOKEN_MINUS_ASSIGN";
        case TOKEN_STAR_ASSIGN:  return "TOKEN_STAR_ASSIGN";
        case TOKEN_SLASH_ASSIGN: return "TOKEN_SLASH_ASSIGN";
        case TOKEN_PERCENT_ASSIGN: return "TOKEN_PERCENT_ASSIGN";

        case TOKEN_LPAREN:    return "TOKEN_LPAREN";
        case TOKEN_RPAREN:    return "TOKEN_RPAREN";
        case TOKEN_LBRACE:    return "TOKEN_LBRACE";
        case TOKEN_RBRACE:    return "TOKEN_RBRACE";
        case TOKEN_LBRACKET:  return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:  return "TOKEN_RBRACKET";
        case TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
        case TOKEN_COMMA:     return "TOKEN_COMMA";
        case TOKEN_DOT:       return "TOKEN_DOT";
        case TOKEN_COLON:     return "TOKEN_COLON";
        case TOKEN_HASH:      return "TOKEN_HASH";
        case TOKEN_AMP:       return "TOKEN_AMP";

        case TOKEN_EOF:     return "TOKEN_EOF";
        case TOKEN_UNKNOWN: return "TOKEN_UNKNOWN";

        default: return "???";
    }
}
