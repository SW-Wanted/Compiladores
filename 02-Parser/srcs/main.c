#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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

    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    return buffer;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: parser <arquivo.c>\n");
        return 1;
    }

    char *source = read_file(argv[1]);
    if (!source) return 1;

    Parser parser;
    parser_init(&parser, source);
    parser_parse(&parser);

    printf("=== AST ===\n");
    parser_print_ast(parser.root, 0);
    printf("===========\n");

    printf("=== TABELA DE SIMBOLOS ===\n");
    scope_table_print(&parser.scope_table);

    parser_free_ast(parser.root);

    free(source);
    return 0;
}
