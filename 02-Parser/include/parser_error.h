#ifndef PARSER_ERROR_H
#define PARSER_ERROR_H

#include "ast.h"

#define MAX_PARSER_ERRORS 256

typedef struct {
    char message[256];
    int  line;
    int  column;
} ParserError;

typedef struct Parser Parser;

ASTNode *parser_error(Parser *parser, const char *message);
void parser_add_error(Parser *parser, const char *message, int line, int column);

#endif
