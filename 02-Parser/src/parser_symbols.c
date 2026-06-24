#include <stdio.h>
#include <string.h>

#include "parser_symbols.h"
#include "parser_utils.h"

const char *declarator_name(const ASTNode *decl)
{
    if (!decl) return NULL;
    if (decl->kind == AST_IDENTIFIER) return decl->text;
    for (int i = 0; i < decl->child_count; i++) {
        const char *name = declarator_name(decl->children[i]);
        if (name) return name;
    }
    return NULL;
}

void declarator_position(const ASTNode *decl, int *line, int *column)
{
    if (!decl) {
        *line = -1;
        *column = -1;
        return;
    }
    if (decl->kind == AST_IDENTIFIER) {
        *line = decl->line;
        *column = decl->column;
        return;
    }
    for (int i = 0; i < decl->child_count; i++) {
        declarator_position(decl->children[i], line, column);
        if (*line >= 0 && *column >= 0)
            return;
    }
    *line = -1;
    *column = -1;
}

int type_specifier_token(const ASTNode *type_spec)
{
    if (!type_spec)
        return TOKEN_IDENTIFIER;

    if (type_spec->child_count > 0 && type_spec->text) {
        if (strcmp(type_spec->text, "const") == 0 || strcmp(type_spec->text, "static") == 0 ||
            strcmp(type_spec->text, "unsigned") == 0 || strcmp(type_spec->text, "signed") == 0 ||
            strcmp(type_spec->text, "volatile") == 0 || strcmp(type_spec->text, "extern") == 0) {
            return type_specifier_token(type_spec->children[0]);
        }
    }

    if (!type_spec->text)
        return TOKEN_IDENTIFIER;
    if (strcmp(type_spec->text, "int") == 0) return TOKEN_INT;
    if (strcmp(type_spec->text, "float") == 0) return TOKEN_FLOAT;
    if (strcmp(type_spec->text, "char") == 0) return TOKEN_CHAR;
    if (strcmp(type_spec->text, "void") == 0) return TOKEN_VOID;
    if (strcmp(type_spec->text, "double") == 0) return TOKEN_DOUBLE;
    if (strcmp(type_spec->text, "long") == 0) return TOKEN_LONG;
    if (strcmp(type_spec->text, "short") == 0) return TOKEN_SHORT;
    if (strcmp(type_spec->text, "struct") == 0) return TOKEN_STRUCT;
    if (strcmp(type_spec->text, "union") == 0) return TOKEN_UNION;
    if (type_spec->child_count > 0)
        return type_specifier_token(type_spec->children[0]);
    return TOKEN_IDENTIFIER;
}

int declarator_pointer_level(const ASTNode *decl)
{
    if (!decl)
        return 0;

    int count = 0;
    if (decl->kind == AST_POINTER)
        return 1;
    for (int i = 0; i < decl->child_count; i++)
        count += declarator_pointer_level(decl->children[i]);
    return count;
}

void build_qualified_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment)
{
    if (!destination || destination_size == 0)
        return;
    if (!parent_scope || !parent_scope[0] || strcmp(parent_scope, "Global") == 0) {
        snprintf(destination, destination_size, "Global::%s", segment ? segment : "");
        return;
    }
    snprintf(destination, destination_size, "%s::%s", parent_scope, segment ? segment : "");
}

void parser_clear_pending_scope(Parser *parser)
{
    parser->pending_scope_name[0] = '\0';
}

void parser_set_pending_scope(Parser *parser, const char *scope_name)
{
    if (!scope_name) {
        parser_clear_pending_scope(parser);
        return;
    }
    strncpy(parser->pending_scope_name, scope_name, MAX_SCOPE_NAME_LEN - 1);
    parser->pending_scope_name[MAX_SCOPE_NAME_LEN - 1] = '\0';
}

void build_child_scope_name(char *destination, size_t destination_size, const char *parent_scope, const char *segment)
{
    if (!parent_scope || !parent_scope[0] || strcmp(parent_scope, "Global") == 0) {
        snprintf(destination, destination_size, "%s", segment);
        return;
    }

    const char *closing = strrchr(parent_scope, ')');
    if (closing && closing > parent_scope) {
        snprintf(destination, destination_size, "%.*s -> %s)", (int)(closing - parent_scope), parent_scope, segment);
        return;
    }

    snprintf(destination, destination_size, "%s (%s)", parent_scope, segment);
}

int insert_symbol(Parser *parser, const ASTNode *type_spec, const ASTNode *decl, SymbolKind kind, int line, int column)
{
    const char *name = declarator_name(decl);
    if (!name) return -1;

    Symbol symbol = {0};
    strncpy(symbol.name, name, MAX_LEXEME_LEN - 1);
    symbol.kind = kind;
    symbol.type_token = type_specifier_token(type_spec);
    symbol.scope_level = parser->scope_table.current_scope;
    if (kind == SYM_FUNCTION) {
        strncpy(symbol.scope_name, "Global", MAX_SCOPE_NAME_LEN - 1);
        symbol.scope_name[MAX_SCOPE_NAME_LEN - 1] = '\0';
    } else if (kind == SYM_PARAM) {
        build_qualified_scope_name(symbol.scope_name, sizeof(symbol.scope_name), "Global", scope_current_name(&parser->scope_table));
    } else {
        strncpy(symbol.scope_name, scope_current_name(&parser->scope_table), MAX_SCOPE_NAME_LEN - 1);
        symbol.scope_name[MAX_SCOPE_NAME_LEN - 1] = '\0';
    }
    symbol.line = line;
    symbol.column = column;
    symbol.param_count = 0;
    if (declarator_pointer_level(decl) > 0) {
        symbol.bytes_size = (int)sizeof(void *);
    } else {
        switch (symbol.type_token) {
            case TOKEN_CHAR: symbol.bytes_size = 1; break;
            case TOKEN_SHORT: symbol.bytes_size = 2; break;
            case TOKEN_INT: symbol.bytes_size = 4; break;
            case TOKEN_LONG: symbol.bytes_size = 8; break;
            case TOKEN_FLOAT: symbol.bytes_size = 4; break;
            case TOKEN_DOUBLE: symbol.bytes_size = 8; break;
            default: symbol.bytes_size = 4; break;
        }
    }
    symbol.mem_address = 0;
    symbol.assigned_value[0] = '\0';
    return scope_insert(&parser->scope_table, &symbol);
}

void insert_declarators(Parser *parser, const ASTNode *type_spec, const ASTNode *node, SymbolKind kind, int line, int column)
{
    if (!node) return;
    if (node->kind == AST_DECLARATOR) {
        insert_symbol(parser, type_spec, node, kind, line, column);
        return;
    }
    if (node->kind == AST_DECLARATOR_LIST) {
        for (int i = 0; i < node->child_count; i++) {
            ASTNode *child = node->children[i];
            if (child->kind == AST_DECLARATOR) {
                int child_line = -1, child_col = -1;
                declarator_position(child, &child_line, &child_col);
                insert_symbol(parser, type_spec, child, kind, child_line, child_col);
                if (i + 1 < node->child_count) {
                    ASTNode *maybe_init = node->children[i + 1];
                    if (maybe_init && (maybe_init->kind == AST_LITERAL || maybe_init->kind == AST_DEFINE_VALUE)) {
                        const char *name = declarator_name(child);
                        if (name) {
                            Symbol *sym = scope_lookup_current(&parser->scope_table, name);
                            if (sym && maybe_init->text) {
                                strncpy(sym->assigned_value, maybe_init->text, sizeof(sym->assigned_value) - 1);
                                sym->assigned_value[sizeof(sym->assigned_value) - 1] = '\0';
                            }
                        }
                    }
                }
            }
        }
    }
}
