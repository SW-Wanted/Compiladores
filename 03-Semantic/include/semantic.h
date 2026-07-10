#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "sem_symtab.h"
#include "sem_error.h"

#define SEM_MAX_TAGS   128
#define SEM_MAX_FIELDS 64

typedef struct {
    char    name[SEM_MAX_NAME];
    int     is_union;
    int     defined;
    int     field_count;
    char    field_names[SEM_MAX_FIELDS][SEM_MAX_NAME];
    SemType field_types[SEM_MAX_FIELDS];
} SemTag;

typedef struct {
    SemSymTab   symtab;
    SemDiagList diags;
    SemTag      tags[SEM_MAX_TAGS];
    int         tag_count;
    int         anon_counter;
    SemType     current_return;
    int         in_function;
    int         loop_depth;
    int         switch_depth;
} SemAnalyzer;

void sem_analyzer_init(SemAnalyzer *analyzer);
int  sem_analyze(SemAnalyzer *analyzer, ASTNode *root);
void sem_print_diagnostics(const SemAnalyzer *analyzer);
void sem_print_symbols(const SemAnalyzer *analyzer);

#endif /* SEMANTIC_H */
