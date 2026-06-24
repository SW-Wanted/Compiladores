#include "parser.h"
#include "parser_declarations.h"
#include "parser_symbols.h"
#include "parser_utils.h"

int parser_parse(Parser *parser)
{
    parser->current = analex(&parser->lexer);
    parser->next = analex(&parser->lexer);
    parser->root = parse_programa(parser);
    return 0;
}

void parser_init(Parser *parser, const char *source)
{
    lexer_init(&parser->lexer, source);
    scope_table_init(&parser->scope_table);
    parser_clear_pending_scope(parser);
    parser->error_count = 0;
}
