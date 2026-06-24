#include <string.h>

#include "parser_error.h"
#include "parser_utils.h"

ASTNode *parser_error(Parser *parser, const char *message)
{
    int line = parser->current.line;
    int column = parser->current.column;

    parser_add_error(parser, message, line, column);
    if (parser->current.type != TOKEN_EOF)
        parser_next(parser);
    return ast_new(AST_ERROR, message, line, column);
}

void parser_add_error(Parser *parser, const char *message, int line, int column)
{
    if (parser->error_count >= MAX_PARSER_ERRORS)
        return;

    ParserError *error = &parser->errors[parser->error_count++];
    strncpy(error->message, message ? message : "erro de sintaxe", sizeof(error->message) - 1);
    error->message[sizeof(error->message) - 1] = '\0';
    error->line = line;
    error->column = column;
}
