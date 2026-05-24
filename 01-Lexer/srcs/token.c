#include "token.h"
const char *token_name(int type)
{
    switch (type) {
        case TOKEN_IDENTIFIER:     return "identificador";
        case TOKEN_INT_LITERAL:    return "literal_int";
        case TOKEN_FLOAT_LITERAL:  return "literal_float";
        case TOKEN_CHAR_LITERAL:   return "literal_char";
        case TOKEN_STRING_LITERAL: return "string_literal";

        case TOKEN_INT:     return "int";
        case TOKEN_FLOAT:   return "float";
        case TOKEN_CHAR:    return "char";
        case TOKEN_VOID:    return "void";
        case TOKEN_IF:      return "if";
        case TOKEN_ELSE:    return "else";
        case TOKEN_WHILE:   return "while";
        case TOKEN_FOR:     return "for";
        case TOKEN_DO:      return "do";
        case TOKEN_RETURN:  return "return";
        case TOKEN_STRUCT:  return "struct";
        case TOKEN_TYPEDEF: return "typedef";
        case TOKEN_INCLUDE: return "include";
        case TOKEN_DEFINE:  return "define";
        case TOKEN_DOUBLE:  return "double";
        case TOKEN_LONG:    return "long";
        case TOKEN_SHORT:   return "short";
        case TOKEN_UNION:   return "union";
        case TOKEN_SWITCH:  return "switch";
        case TOKEN_CASE:    return "case";
        case TOKEN_DEFAULT: return "default";
        case TOKEN_BREAK:   return "break";
        case TOKEN_CONTINUE:return "continue";
        case TOKEN_SIZEOF:  return "sizeof";

        case TOKEN_PLUS:    return "+";
        case TOKEN_MINUS:   return "-";
        case TOKEN_STAR:    return "*";
        case TOKEN_SLASH:   return "/";
        case TOKEN_PERCENT: return "%";
        case TOKEN_PLUS_PLUS: return "++";
        case TOKEN_MINUS_MINUS: return "--";
        case TOKEN_LSHIFT:  return "<<";
        case TOKEN_RSHIFT:  return ">>";
        case TOKEN_ARROW:   return "->";

        case TOKEN_EQ:  return "==";
        case TOKEN_NEQ: return "!=";
        case TOKEN_LT:  return "<";
        case TOKEN_GT:  return ">";
        case TOKEN_LEQ: return "<=";
        case TOKEN_GEQ: return ">=";

        case TOKEN_AND: return "&&";
        case TOKEN_OR:  return "||";
        case TOKEN_NOT: return "!";
        case TOKEN_PIPE: return "|";
        case TOKEN_CARET:return "^";

        case TOKEN_ASSIGN:       return "=";
        case TOKEN_PLUS_ASSIGN:  return "+=";
        case TOKEN_MINUS_ASSIGN: return "-=";
        case TOKEN_STAR_ASSIGN:  return "*=";
        case TOKEN_SLASH_ASSIGN: return "/=";
        case TOKEN_PERCENT_ASSIGN: return "%=";
        case TOKEN_AMP_ASSIGN:   return "&=";
        case TOKEN_PIPE_ASSIGN:  return "|=";
        case TOKEN_CARET_ASSIGN: return "^=";
        case TOKEN_LSHIFT_ASSIGN:return "<<=";
        case TOKEN_RSHIFT_ASSIGN:return ">>=";
        case TOKEN_QUESTION: return "?";

        case TOKEN_LPAREN:    return "(";
        case TOKEN_RPAREN:    return ")";
        case TOKEN_LBRACE:    return "{";
        case TOKEN_RBRACE:    return "}";
        case TOKEN_LBRACKET:  return "[";
        case TOKEN_RBRACKET:  return "]";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_COMMA:     return ",";
        case TOKEN_DOT:       return ".";
        case TOKEN_COLON:     return ":";
        case TOKEN_HASH:      return "#";
        case TOKEN_AMP:       return "&";

        case TOKEN_EOF:     return "EOF";
        case TOKEN_UNKNOWN: return "desconhecido";

        default: return "???";
    }
}
