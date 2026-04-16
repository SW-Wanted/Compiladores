#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

typedef struct { const char *word; int type; } Keyword;

static const Keyword KEYWORDS[] = {
    { "int",     TOKEN_INT     },
    { "float",   TOKEN_FLOAT   },
    { "char",    TOKEN_CHAR    },
    { "void",    TOKEN_VOID    },
    { "if",      TOKEN_IF      },
    { "else",    TOKEN_ELSE    },
    { "while",   TOKEN_WHILE   },
    { "for",     TOKEN_FOR     },
    { "do",      TOKEN_DO      },
    { "return",  TOKEN_RETURN  },
    { "struct",  TOKEN_STRUCT  },
    { "typedef", TOKEN_TYPEDEF },
    { "include", TOKEN_INCLUDE },
    { "define",  TOKEN_DEFINE  },
    { NULL, 0 }
};

static int lookup_keyword(const char *word)
{
    for (int i = 0; KEYWORDS[i].word != NULL; i++) {
        if (strcmp(KEYWORDS[i].word, word) == 0)
            return KEYWORDS[i].type;
    }
    return TOKEN_IDENTIFIER;
}

static char peek(const Lexer *lexer)
{
    return lexer->source[lexer->pos];
}

static Token make_token(int type, const char *lexeme, int line)
{
    Token t;
    t.type = type;
    t.line = line;
    strncpy(t.lexeme, lexeme, MAX_LEXEME_LEN - 1);
    t.lexeme[MAX_LEXEME_LEN - 1] = '\0';
    return t;
}

void lexer_init(Lexer *lexer, const char *source)
{
    lexer->source = source;
    lexer->pos    = 0;
    lexer->line   = 1;
}

char ler_caractere(Lexer *lexer)
{
    char c = lexer->source[lexer->pos];
    if (c == '\n') lexer->line++;
    lexer->pos++;
    return c;
}

void volta_caractere(Lexer *lexer)
{
    if (lexer->pos > 0) {
        lexer->pos--;
        if (lexer->source[lexer->pos] == '\n')
            lexer->line--;
    }
}

void gravar_token_lexema(SymbolTable *table, Token token)
{
    symtable_insert(table, token);
}

static Token scan_identifier(Lexer *lexer)
{
    char buf[MAX_LEXEME_LEN];
    int  len  = 0;
    int  line = lexer->line;

    char c;
    while (isalnum(c = ler_caractere(lexer)) || c == '_') {
        if (len < MAX_LEXEME_LEN - 1)
            buf[len++] = c;
    }
    volta_caractere(lexer);
    buf[len] = '\0';

    int type = lookup_keyword(buf);
    return make_token(type, buf, line);
}

static Token scan_number(Lexer *lexer)
{
    char buf[MAX_LEXEME_LEN];
    int  len   = 0;
    int  line  = lexer->line;
    int  is_float = 0;

    char c;
    while (isdigit(c = ler_caractere(lexer))) {
        if (len < MAX_LEXEME_LEN - 1) buf[len++] = c;
    }

    if (c == '.' && isdigit(peek(lexer))) {
        is_float = 1;
        if (len < MAX_LEXEME_LEN - 1) buf[len++] = c;
        while (isdigit(c = ler_caractere(lexer))) {
            if (len < MAX_LEXEME_LEN - 1) buf[len++] = c;
        }
    }

    volta_caractere(lexer);
    buf[len] = '\0';

    int type = is_float ? TOKEN_FLOAT_LITERAL : TOKEN_INT_LITERAL;
    return make_token(type, buf, line);
}

static Token scan_char_literal(Lexer *lexer)
{
    char buf[8];
    int  line = lexer->line;

    buf[0] = '\'';
    buf[1] = ler_caractere(lexer);
    if (buf[1] == '\\')
        buf[2] = ler_caractere(lexer);
    buf[3] = ler_caractere(lexer);
    buf[4] = '\0';

    return make_token(TOKEN_CHAR_LITERAL, buf, line);
}

static Token scan_string_literal(Lexer *lexer)
{
    char buf[MAX_LEXEME_LEN];
    int  len  = 0;
    int  line = lexer->line;

    buf[len++] = '"';
    char c;
    while ((c = ler_caractere(lexer)) != '"' && c != '\0') {
        if (c == '\\') {
            if (len < MAX_LEXEME_LEN - 2) { buf[len++] = c; }
            c = ler_caractere(lexer);
        }
        if (len < MAX_LEXEME_LEN - 2) buf[len++] = c;
    }
    if (len < MAX_LEXEME_LEN - 1) buf[len++] = '"';
    buf[len] = '\0';

    return make_token(TOKEN_STRING_LITERAL, buf, line);
}

static void skip_line_comment(Lexer *lexer)
{
    char c;
    while ((c = ler_caractere(lexer)) != '\n' && c != '\0');
}

static void skip_block_comment(Lexer *lexer)
{
    char c;
    while ((c = ler_caractere(lexer)) != '\0') {
        if (c == '*' && peek(lexer) == '/') {
            ler_caractere(lexer);
            return;
        }
    }
}

Token analex(Lexer *lexer)
{
    char c;
    do {
        c = ler_caractere(lexer);
    } while (c == ' ' || c == '\t' || c == '\r' || c == '\n');

    int line = lexer->line;

    if (c == '\0') return make_token(TOKEN_EOF, "EOF", line);

    if (isalpha(c) || c == '_') {
        volta_caractere(lexer);
        return scan_identifier(lexer);
    }

    if (isdigit(c)) {
        volta_caractere(lexer);
        return scan_number(lexer);
    }

    if (c == '"') return scan_string_literal(lexer);

    if (c == '\'') {
        volta_caractere(lexer);
        ler_caractere(lexer);
        return scan_char_literal(lexer);
    }

    char next = peek(lexer);
    char buf[3] = { c, '\0', '\0' };

    switch (c) {
        case '/':
            if (next == '/') { ler_caractere(lexer); skip_line_comment(lexer);  return analex(lexer); }
            if (next == '*') { ler_caractere(lexer); skip_block_comment(lexer); return analex(lexer); }
            return make_token(TOKEN_SLASH, "/", line);

        case '+':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_PLUS_ASSIGN,  "+=", line); }
            return make_token(TOKEN_PLUS,  "+", line);

        case '-':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_MINUS_ASSIGN, "-=", line); }
            return make_token(TOKEN_MINUS, "-", line);

        case '*': return make_token(TOKEN_STAR,    "*", line);
        case '%': return make_token(TOKEN_PERCENT, "%", line);

        case '=':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_EQ,  "==", line); }
            return make_token(TOKEN_ASSIGN, "=", line);

        case '!':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_NEQ, "!=", line); }
            return make_token(TOKEN_NOT, "!", line);

        case '<':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_LEQ, "<=", line); }
            return make_token(TOKEN_LT, "<", line);

        case '>':
            if (next == '=') { ler_caractere(lexer); return make_token(TOKEN_GEQ, ">=", line); }
            return make_token(TOKEN_GT, ">", line);

        case '&':
            if (next == '&') { ler_caractere(lexer); return make_token(TOKEN_AND, "&&", line); }
            return make_token(TOKEN_AMP, "&", line);

        case '|':
            if (next == '|') { ler_caractere(lexer); return make_token(TOKEN_OR, "||", line); }
            break;

        case '(': return make_token(TOKEN_LPAREN,    "(", line);
        case ')': return make_token(TOKEN_RPAREN,    ")", line);
        case '{': return make_token(TOKEN_LBRACE,    "{", line);
        case '}': return make_token(TOKEN_RBRACE,    "}", line);
        case '[': return make_token(TOKEN_LBRACKET,  "[", line);
        case ']': return make_token(TOKEN_RBRACKET,  "]", line);
        case ';': return make_token(TOKEN_SEMICOLON, ";", line);
        case ',': return make_token(TOKEN_COMMA,     ",", line);
        case '.': return make_token(TOKEN_DOT,       ".", line);
        case ':': return make_token(TOKEN_COLON,     ":", line);
        case '#': return make_token(TOKEN_HASH,      "#", line);
    }

    buf[0] = c;
    buf[1] = '\0';
    return make_token(TOKEN_UNKNOWN, buf, line);
}

void lexer_run(Lexer *lexer, SymbolTable *table)
{
    Token token;
    do {
        token = analex(lexer);
        gravar_token_lexema(table, token);
    } while (token.type != TOKEN_EOF);
}
