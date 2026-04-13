#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "symbol_table.h"

/* =========================================================
 *  Reads an entire file into a heap-allocated string.
 *  Caller is responsible for calling free() on the result.
 * ========================================================= */
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

/* =========================================================
 *  MAIN
 * ========================================================= */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: lexer <arquivo.c>\n");
        return 1;
    }

    /* 1. Read source file */
    char *source = read_file(argv[1]);
    if (!source) return 1;

    printf("Analisando: %s\n", argv[1]);
    printf("================================================\n");

    /* 2. Initialize lexer and symbol table */
    Lexer       lexer;
    SymbolTable table;

    lexer_init    (&lexer, source);
    symtable_init (&table);

    /* 3. Run the lexer — fills the symbol table */
    lexer_run(&lexer, &table);

    /* 4. Print the symbol table */
    symtable_print(&table);

    free(source);
    return 0;
}
