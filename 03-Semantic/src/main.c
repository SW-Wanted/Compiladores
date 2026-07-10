#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "semantic.h"

typedef struct {
    int show_ast;
    int show_symbols;
    int show_diagnostics;
    int show_help;
} OutputFlags;

/**
 * @brief Devolve o nome do programa sem o caminho.
 */
static const char *program_name(const char *path)
{
    const char *slash = strrchr(path, '/');
    return slash ? slash + 1 : path;
}

/**
 * @brief Imprime a ajuda no formato de ferramenta de linha de comandos.
 */
static void print_help(FILE *out, const char *prog)
{
    fprintf(out, "Fase 03 - Analisador Semantico\n\n");
    fprintf(out, "USO\n");
    fprintf(out, "    %s [OPCOES] <arquivo.c>\n\n", prog);
    fprintf(out, "OPCOES\n");
    fprintf(out, "    -d, --diagnostics   Mostrar diagnosticos semanticos (predefinido)\n");
    fprintf(out, "    -s, --symbols       Mostrar tabela de simbolos\n");
    fprintf(out, "    -a, --ast           Mostrar arvore sintactica\n");
    fprintf(out, "    -A, --all           Mostrar todas as informacoes\n");
    fprintf(out, "    -h, --help          Mostrar esta ajuda\n\n");
    fprintf(out, "EXEMPLOS\n");
    fprintf(out, "    %s teste.c\n", prog);
    fprintf(out, "    %s --all teste.c\n", prog);
    fprintf(out, "    %s --symbols teste.c\n", prog);
}

/**
 * @brief Le o conteudo de um ficheiro para uma string alocada dinamicamente.
 */
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

/**
 * @brief Interpreta os argumentos da linha de comandos.
 */
static OutputFlags parse_output_flags(int argc, char *argv[], const char **filename)
{
    OutputFlags flags = {0, 0, 0, 0};

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
        } else if (strcmp(argv[i], "--help") == 0) {
            flags.show_help = 1;
        } else {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'a': flags.show_ast = 1; break;
                    case 's': flags.show_symbols = 1; break;
                    case 'd': flags.show_diagnostics = 1; break;
                    case 'A': flags.show_ast = flags.show_symbols = flags.show_diagnostics = 1; break;
                    case 'h': flags.show_help = 1; break;
                    default: break;
                }
            }
        }
    }

    if (!flags.show_ast && !flags.show_symbols && !flags.show_diagnostics)
        flags.show_diagnostics = 1;

    return flags;
}

/**
 * @brief Imprime a tabela de erros sintacticos que impediram a analise semantica.
 */
static void print_syntax_errors(const Parser *parser)
{
    printf("=== ERROS SINTACTICOS ===\n");
    printf("%-4s %-6s %-7s %s\n", "#", "LINHA", "COLUNA", "MENSAGEM");
    printf("%-4s %-6s %-7s %s\n", "----", "------", "-------",
           "--------------------------------------------------");
    for (int i = 0; i < parser->error_count; i++)
        printf("%-4d %-6d %-7d \x1b[31m%s\x1b[0m\n",
               i + 1, parser->errors[i].line, parser->errors[i].column,
               parser->errors[i].message);
    printf("\nTotal de erros sintacticos: %d\n", parser->error_count);
    printf("Analise semantica abortada (corrija primeiro os erros de sintaxe).\n");
}

/**
 * @brief Ponto de entrada: liga o Parser ao Analisador Semantico.
 */
int main(int argc, char *argv[])
{
    const char *prog = program_name(argv[0]);

    if (argc < 2) {
        print_help(stderr, prog);
        return 1;
    }

    const char *filename = NULL;
    OutputFlags flags = parse_output_flags(argc, argv, &filename);

    if (flags.show_help) {
        print_help(stdout, prog);
        return 0;
    }
    if (!filename) {
        print_help(stderr, prog);
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
