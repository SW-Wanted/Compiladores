#include <string.h>
#include <stdio.h>

#include "semantic.h"
#include "sem_check.h"

/* ------------------------------------------------------------------ */
/*  Registo de builtins (biblioteca padrao)                            */
/* ------------------------------------------------------------------ */

static void builtin_function(SemAnalyzer *a, const char *name, SemType ret)
{
    SemSymbol sym;
    memset(&sym, 0, sizeof(sym));
    strncpy(sym.name, name, SEM_MAX_NAME - 1);
    sym.kind = SYM_FUNCTION;
    sym.type = ret;
    sym.is_variadic = 1;   /* nao verificar argumentos de funcoes externas */
    sym.is_defined = 0;
    sym.line = 0;
    sym.column = 0;
    int duplicate = 0;
    sem_declare(&a->symtab, &sym, &duplicate);
}

static void builtin_const(SemAnalyzer *a, const char *name, SemType type)
{
    SemSymbol sym;
    memset(&sym, 0, sizeof(sym));
    strncpy(sym.name, name, SEM_MAX_NAME - 1);
    sym.kind = SYM_ENUM_CONST;
    sym.type = type;
    sym.is_initialized = 1;
    int duplicate = 0;
    sem_declare(&a->symtab, &sym, &duplicate);
}

static void register_builtins(SemAnalyzer *a)
{
    SemType t_int = sem_type_int();
    SemType t_double = sem_type_make(TOKEN_DOUBLE, NULL);
    SemType t_long = sem_type_make(TOKEN_LONG, NULL);
    SemType t_void = sem_type_void();
    SemType t_voidp = sem_type_void(); t_voidp.pointer_level = 1;
    SemType t_charp = sem_type_make(TOKEN_CHAR, NULL); t_charp.pointer_level = 1;

    /* Entrada/saida (stdio.h) */
    const char *io_int[] = { "printf", "scanf", "fprintf", "fscanf", "sprintf",
                             "sscanf", "snprintf", "puts", "fputs", "putchar",
                             "getchar", "putc", "getc", "fclose", "fflush",
                             "fread", "fwrite", "remove", "rename", NULL };
    for (int i = 0; io_int[i]; i++)
        builtin_function(a, io_int[i], t_int);
    builtin_function(a, "fopen", t_voidp);
    builtin_function(a, "fgets", t_charp);
    builtin_function(a, "gets", t_charp);

    /* Memoria e conversao (stdlib.h) */
    builtin_function(a, "malloc", t_voidp);
    builtin_function(a, "calloc", t_voidp);
    builtin_function(a, "realloc", t_voidp);
    builtin_function(a, "free", t_void);
    builtin_function(a, "exit", t_void);
    builtin_function(a, "abort", t_void);
    builtin_function(a, "atoi", t_int);
    builtin_function(a, "atof", t_double);
    builtin_function(a, "atol", t_long);
    builtin_function(a, "rand", t_int);
    builtin_function(a, "srand", t_void);
    builtin_function(a, "system", t_int);
    builtin_function(a, "abs", t_int);
    builtin_function(a, "labs", t_long);

    /* Strings (string.h) */
    const char *str_int[] = { "strcmp", "strncmp", "strlen", "memcmp", NULL };
    for (int i = 0; str_int[i]; i++)
        builtin_function(a, str_int[i], t_int);
    const char *str_ptr[] = { "strcpy", "strncpy", "strcat", "strncat",
                              "strchr", "strrchr", "strstr", "memcpy",
                              "memset", "memmove", NULL };
    for (int i = 0; str_ptr[i]; i++)
        builtin_function(a, str_ptr[i], t_charp);

    /* Matematica (math.h) */
    const char *math_d[] = { "sqrt", "pow", "sin", "cos", "tan", "exp", "log",
                             "log10", "fabs", "floor", "ceil", "fmod", NULL };
    for (int i = 0; math_d[i]; i++)
        builtin_function(a, math_d[i], t_double);

    /* Constantes/macros comuns */
    builtin_const(a, "NULL", t_voidp);
    builtin_const(a, "EOF", t_int);
    builtin_const(a, "stdin", t_voidp);
    builtin_const(a, "stdout", t_voidp);
    builtin_const(a, "stderr", t_voidp);
}

void sem_analyzer_init(SemAnalyzer *a)
{
    sem_symtab_init(&a->symtab);
    sem_diag_init(&a->diags);
    a->tag_count = 0;
    a->anon_counter = 0;
    a->current_return = sem_type_void();
    a->in_function = 0;
    a->loop_depth = 0;
    a->switch_depth = 0;
    register_builtins(a);
}

/* ------------------------------------------------------------------ */
/*  Impressao de resultados                                            */
/* ------------------------------------------------------------------ */

static const char *kind_name(SymbolKind kind)
{
    switch (kind) {
        case SYM_VARIABLE:   return "VARIAVEL";
        case SYM_FUNCTION:   return "FUNCAO";
        case SYM_TYPEDEF:    return "TIPODEF";
        case SYM_PARAM:      return "PARAMETRO";
        case SYM_ENUM_CONST: return "CONSTANTE";
        default:             return "DESCONHECIDO";
    }
}

void sem_print_symbols(const SemAnalyzer *a)
{
    printf("\n%-4s %-18s %-11s %-16s %-6s %-6s %-6s\n",
           "#", "IDENTIFICADOR", "ESPECIE", "TIPO", "NIVEL", "LINHA", "COLUNA");
    printf("%-4s %-18s %-11s %-16s %-6s %-6s %-6s\n",
           "----", "------------------", "-----------", "----------------",
           "------", "------", "------");
    for (int i = 0; i < a->symtab.count; i++) {
        const SemSymbol *s = &a->symtab.symbols[i];
        if (s->line == 0 && s->kind != SYM_VARIABLE)
            continue; /* nao listar builtins (linha 0) */
        char type_str[SEM_MAX_NAME + 32];
        sem_type_to_string(&s->type, type_str, sizeof(type_str));
        printf("%-4d %-18s %-11s %-16s %-6d %-6d %-6d\n",
               i + 1, s->name, kind_name(s->kind), type_str,
               s->scope_level, s->line, s->column);
    }
    printf("\n");
}

void sem_print_diagnostics(const SemAnalyzer *a)
{
    const SemDiagList *d = &a->diags;

    if (d->count == 0) {
        printf("\x1b[32mAnalise semantica concluida com sucesso: 0 erros, 0 avisos.\x1b[0m\n");
        return;
    }

    printf("=== DIAGNOSTICOS SEMANTICOS ===\n");
    printf("%-4s %-9s %-58s %-6s %-6s\n", "#", "TIPO", "MENSAGEM", "LINHA", "COLUNA");
    printf("%-4s %-9s %-58s %-6s %-6s\n", "----", "---------",
           "----------------------------------------------------------",
           "------", "------");
    for (int i = 0; i < d->count; i++) {
        const SemDiagnostic *item = &d->items[i];
        const char *color = item->severity == SEM_SEV_ERROR ? "\x1b[31m" : "\x1b[33m";
        const char *label = item->severity == SEM_SEV_ERROR ? "ERRO" : "AVISO";
        printf("%-4d %s%-9s %-58s\x1b[0m %-6d %-6d\n",
               i + 1, color, label, item->message, item->line, item->column);
    }
    printf("\nTotal: %d erro(s), %d aviso(s).\n", d->error_count, d->warning_count);
}
