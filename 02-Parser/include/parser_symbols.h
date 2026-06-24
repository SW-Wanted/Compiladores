#ifndef PARSER_SYMBOLS_H
#define PARSER_SYMBOLS_H

#include <stddef.h>

#include "parser.h"

const char *declarator_name(const ASTNode *decl);
void declarator_position(const ASTNode *decl, int *line, int *column);
int declarator_pointer_level(const ASTNode *decl);
int type_specifier_token(const ASTNode *type_spec);
int insert_symbol(Parser *parser, const ASTNode *type_spec, const ASTNode *decl, SymbolKind kind, int line, int column);
void insert_declarators(Parser *parser, const ASTNode *type_spec, const ASTNode *node, SymbolKind kind, int line, int column);
void parser_clear_pending_scope(Parser *parser);
void parser_set_pending_scope(Parser *parser, const char *scope_name);
void build_qualified_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment);
void build_child_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment);

#endif
