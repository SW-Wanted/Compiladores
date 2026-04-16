#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "symbol_table.h"

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
        fprintf(stderr, "Uso: lexer <arquivo.c>\n");
        return 1;
    }

    char *source = read_file(argv[1]);
    if (!source) return 1;

    printf("Analisando: %s\n", argv[1]);
    printf("================================================\n");

    Lexer       lexer;
    SymbolTable table;

    lexer_init(&lexer, source);
    symtable_init(&table);

    lexer_run(&lexer, &table);
    symtable_print(&table);

    free(source);
    return 0;
}
