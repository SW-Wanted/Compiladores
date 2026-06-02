#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "symbol_table.h"

typedef struct {
    int show_ast;
    int show_symbols;
    int show_errors;
    int show_lexemes;
} OutputFlags;

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Uso: %s [opcoes] <arquivo.c>\n", program_name);
    fprintf(stderr, "Opcoes:\n");
    fprintf(stderr, "  -a, --ast       mostrar AST\n");
    fprintf(stderr, "  -s, --symbols   mostrar tabela de simbolos\n");
    fprintf(stderr, "  -e, --errors    mostrar tabela de erros\n");
    fprintf(stderr, "  -l, --lexemes   mostrar lexemas\n");
    fprintf(stderr, "  -A, --all       mostrar tudo\n");
}

static void print_ast_view(const Parser *parser)
{
    printf("=== AST ===\n");
    parser_print_ast(parser->root, 0);
    printf("===========\n");
}

static void print_symbols_view(const Parser *parser)
{
    printf("=== TABELA DE SIMBOLOS ===\n");
    scope_table_print(&parser->scope_table);
}

static void print_lexemes_view(const char *source)
{
    Lexer lexer;
    SymbolTable table;

    lexer_init(&lexer, source);
    symtable_init(&table);
    lexer_run(&lexer, &table);
    printf("=== LEXEMAS ===\n");
    symtable_print(&table);
    printf("===========\n");
}

static void print_errors_view(const Parser *parser)
{
    if (parser->error_count == 0) {
        printf("Total de erros: 0\n");
        return;
    }

    printf("=== TABELA DE ERROS ===\n");
    printf("%-5s %-40s %-7s %-7s\n", "#", "MENSAGEM", "LINHA", "COLUNA");
    printf("%-5s %-40s %-7s %-7s\n", "-----", "----------------------------------------", "-------", "-------");
    for (int i = 0; i < parser->error_count; i++) {
        printf("%-5d \x1b[31m%-40s\x1b[0m %-7d %-7d\n",
               i + 1,
               parser->errors[i].message,
               parser->errors[i].line,
               parser->errors[i].column);
    }
    printf("\nTotal de erros: %d\n", parser->error_count);
}

static OutputFlags parse_output_flags(int argc, char *argv[], const char **filename)
{
    OutputFlags flags = {0, 0, 0, 0};

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "--ast") == 0) {
                flags.show_ast = 1;
            } else if (strcmp(argv[i], "--symbols") == 0) {
                flags.show_symbols = 1;
            } else if (strcmp(argv[i], "--errors") == 0) {
                flags.show_errors = 1;
            } else if (strcmp(argv[i], "--lexemes") == 0) {
                flags.show_lexemes = 1;
            } else if (strcmp(argv[i], "--all") == 0) {
                flags.show_ast = 1;
                flags.show_symbols = 1;
                flags.show_errors = 1;
                flags.show_lexemes = 1;
            } else if (argv[i][1] != '\0' && argv[i][1] != '-') {
                for (int j = 1; argv[i][j] != '\0'; j++) {
                    switch (argv[i][j]) {
                        case 'a':
                            flags.show_ast = 1;
                            break;
                        case 's':
                            flags.show_symbols = 1;
                            break;
                        case 'e':
                            flags.show_errors = 1;
                            break;
                        case 'l':
                            flags.show_lexemes = 1;
                            break;
                        case 'A':
                            flags.show_ast = 1;
                            flags.show_symbols = 1;
                            flags.show_errors = 1;
                            flags.show_lexemes = 1;
                            break;
                        default:
                            break;
                    }
                }
            } else {
                continue;
            }
        } else {
            *filename = argv[i];
        }
    }

    if (!flags.show_ast && !flags.show_symbols && !flags.show_errors && !flags.show_lexemes)
        flags.show_errors = 1;

    return flags;
}

char *read_file(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Erro: nao foi possivel abrir '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    return buffer;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = NULL;
    OutputFlags flags = parse_output_flags(argc, argv, &filename);

    if (!filename) {
        print_usage(argv[0]);
        return 1;
    }

    char *source = read_file(filename);
    if (!source) return 1;

    Parser parser;
    parser_init(&parser, source);
    parser_parse(&parser);

    if (flags.show_lexemes)
        print_lexemes_view(source);

    if (flags.show_ast)
        print_ast_view(&parser);

    if (flags.show_symbols)
        print_symbols_view(&parser);

    if (flags.show_errors)
        print_errors_view(&parser);

    parser_free_ast(parser.root);

    free(source);
    return 0;
}
