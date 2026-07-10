#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "semantic.h"

typedef struct {
    int show_ast;
    int show_symbols;
    int show_diagnostics;
} OutputFlags;

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Uso: %s [opcoes] <arquivo.c>\n", program_name);
    fprintf(stderr, "Opcoes:\n");
    fprintf(stderr, "  -a, --ast        mostrar a arvore sintactica (AST)\n");
    fprintf(stderr, "  -s, --symbols    mostrar a tabela de simbolos semantica\n");
    fprintf(stderr, "  -d, --diagnostics mostrar os diagnosticos semanticos (predefinido)\n");
    fprintf(stderr, "  -A, --all        mostrar tudo\n");
}

static char *read_file(const char *path)
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

    size_t read = fread(buffer, 1, size, f);
    buffer[read] = '\0';
    fclose(f);
    return buffer;
}

static OutputFlags parse_output_flags(int argc, char *argv[], const char **filename)
{
    OutputFlags flags = {0, 0, 0};

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            *filename = argv[i];
            continue;
        }
        if (strcmp(argv[i], "--ast") == 0) {
            flags.show_ast = 1;
        } else if (strcmp(argv[i], "--symbols") == 0) {
            flags.show_symbols = 1;
        } else if (strcmp(argv[i], "--diagnostics") == 0) {
            flags.show_diagnostics = 1;
        } else if (strcmp(argv[i], "--all") == 0) {
            flags.show_ast = flags.show_symbols = flags.show_diagnostics = 1;
        } else {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'a': flags.show_ast = 1; break;
                    case 's': flags.show_symbols = 1; break;
                    case 'd': flags.show_diagnostics = 1; break;
                    case 'A': flags.show_ast = flags.show_symbols = flags.show_diagnostics = 1; break;
                    default: break;
                }
            }
        }
    }

    if (!flags.show_ast && !flags.show_symbols && !flags.show_diagnostics)
        flags.show_diagnostics = 1;

    return flags;
}

static void print_syntax_errors(const Parser *parser)
{
    printf("=== ERROS SINTACTICOS ===\n");
    printf("%-5s %-40s %-7s %-7s\n", "#", "MENSAGEM", "LINHA", "COLUNA");
    printf("%-5s %-40s %-7s %-7s\n", "-----",
           "----------------------------------------", "-------", "-------");
    for (int i = 0; i < parser->error_count; i++)
        printf("%-5d \x1b[31m%-40s\x1b[0m %-7d %-7d\n",
               i + 1, parser->errors[i].message,
               parser->errors[i].line, parser->errors[i].column);
    printf("\nTotal de erros sintacticos: %d\n", parser->error_count);
    printf("Analise semantica abortada (corrija primeiro os erros de sintaxe).\n");
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
    if (!source)
        return 1;

    Parser parser;
    parser_init(&parser, source);
    parser_parse(&parser);

    if (flags.show_ast) {
        printf("=== AST ===\n");
        parser_print_ast(parser.root, 0);
        printf("===========\n");
    }

    int exit_code = 0;

    if (parser.error_count > 0) {
        print_syntax_errors(&parser);
        exit_code = 1;
    } else {
        /* A estrutura do analisador e grande (tabelas de simbolos e tipos);
           aloca-se no heap para nao esgotar a pilha. */
        SemAnalyzer *analyzer = malloc(sizeof(SemAnalyzer));
        if (!analyzer) {
            fprintf(stderr, "Erro: memoria insuficiente\n");
            parser_free_ast(parser.root);
            free(source);
            return 1;
        }
        sem_analyzer_init(analyzer);
        int errors = sem_analyze(analyzer, parser.root);

        if (flags.show_symbols)
            sem_print_symbols(analyzer);
        if (flags.show_diagnostics)
            sem_print_diagnostics(analyzer);

        exit_code = errors > 0 ? 1 : 0;
        free(analyzer);
    }

    parser_free_ast(parser.root);
    free(source);
    return exit_code;
}
