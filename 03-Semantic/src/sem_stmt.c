#include <string.h>
#include <stdio.h>

#include "sem_check.h"

/* ------------------------------------------------------------------ */
/*  Declaracao de variaveis                                            */
/* ------------------------------------------------------------------ */

static SemSymbol *declare_variable(SemAnalyzer *a, SemType base, ASTNode *declarator)
{
    const char *name = sem_declarator_name(declarator);
    int line = -1, column = -1;
    sem_node_position(declarator, &line, &column);
    if (!name)
        return NULL;

    SemType type = sem_type_from_declarator(base, declarator);

    if (sem_type_is_void(&type)) {
        sem_error(&a->diags, line, column,
                  "variavel '%s' nao pode ter tipo 'void'", name);
    }

    SemSymbol symbol;
    memset(&symbol, 0, sizeof(symbol));
    strncpy(symbol.name, name, SEM_MAX_NAME - 1);
    symbol.kind = SYM_VARIABLE;
    symbol.type = type;
    symbol.line = line;
    symbol.column = column;

    int duplicate = 0;
    SemSymbol *slot = sem_declare(&a->symtab, &symbol, &duplicate);
    if (duplicate && slot) {
        sem_error(&a->diags, line, column,
                  "redeclaracao de '%s' no mesmo escopo (declaracao anterior na linha %d)",
                  name, slot->line);
    }
    return slot;
}

/* Verifica a compatibilidade do inicializador de um declarador. */
static void check_initializer(SemAnalyzer *a, SemSymbol *sym, SemType type, ASTNode *init)
{
    SemType it = sem_check_expr(a, init);
    char ctx[SEM_MAX_NAME + 32];
    int line = sem_line(init), col = sem_col(init);
    if (line < 0) { line = sym ? sym->line : -1; col = sym ? sym->column : -1; }
    snprintf(ctx, sizeof(ctx), "inicializacao de '%s'", sym ? sym->name : "?");
    sem_check_assign_compat(a, type, it, init, line, col, ctx);
    if (sym) sym->is_initialized = 1;
}

void sem_check_var_decl(SemAnalyzer *a, ASTNode *node, int is_global)
{
    (void)is_global;
    if (node->child_count < 2)
        return;

    SemType base = sem_type_from_spec(a, node->children[0]);
    ASTNode *first_decl = node->children[1];

    SemSymbol *first = declare_variable(a, base, first_decl);
    SemType first_type = first ? first->type : sem_type_from_declarator(base, first_decl);

    int idx = 2;

    /* Inicializador do primeiro declarador (se houver). */
    if (idx < node->child_count && node->children[idx]->kind != AST_DECLARATOR_LIST) {
        check_initializer(a, first, first_type, node->children[idx]);
        idx++;
    }

    /* Declaradores adicionais: `, decl [= init]`. */
    if (idx < node->child_count && node->children[idx]->kind == AST_DECLARATOR_LIST) {
        ASTNode *more = node->children[idx];
        for (int i = 0; i < more->child_count; i++) {
            ASTNode *declarator = more->children[i];
            if (declarator->kind != AST_DECLARATOR)
                continue;
            SemSymbol *sym = declare_variable(a, base, declarator);
            SemType dtype = sym ? sym->type : sem_type_from_declarator(base, declarator);
            if (i + 1 < more->child_count && more->children[i + 1]->kind != AST_DECLARATOR) {
                check_initializer(a, sym, dtype, more->children[i + 1]);
                i++;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Condicoes de estruturas de controlo                                */
/* ------------------------------------------------------------------ */

static void check_condition(SemAnalyzer *a, ASTNode *expr, const char *keyword)
{
    if (!expr)
        return;
    SemType t = sem_check_expr(a, expr);
    if (t.valid && !sem_type_is_scalar(&t)) {
        char s[SEM_MAX_NAME + 32];
        sem_type_to_string(&t, s, sizeof(s));
        sem_error(&a->diags, sem_line(expr), sem_col(expr),
                  "a condicao de '%s' deve ser de tipo escalar (obtido '%s')", keyword, s);
    }
}

/* ------------------------------------------------------------------ */
/*  Blocos e instrucoes                                                */
/* ------------------------------------------------------------------ */

void sem_check_block(SemAnalyzer *a, ASTNode *block, int new_scope)
{
    if (!block)
        return;
    if (new_scope)
        sem_scope_enter(&a->symtab);

    for (int i = 0; i < block->child_count; i++) {
        ASTNode *child = block->children[i];
        if (child->kind == AST_VAR_DECL)
            sem_check_var_decl(a, child, 0);
        else
            sem_check_stmt(a, child);
    }

    if (new_scope)
        sem_scope_exit(&a->symtab);
}

static void check_switch(SemAnalyzer *a, ASTNode *node)
{
    if (node->child_count > 0) {
        SemType t = sem_check_expr(a, node->children[0]);
        if (t.valid && !sem_type_is_integer(&t))
            sem_error(&a->diags, sem_line(node->children[0]), sem_col(node->children[0]),
                      "a expressao de 'switch' deve ser de tipo inteiro");
    }
    a->switch_depth++;
    for (int i = 1; i < node->child_count; i++)
        sem_check_stmt(a, node->children[i]);
    a->switch_depth--;
}

static void check_for(SemAnalyzer *a, ASTNode *node)
{
    sem_scope_enter(&a->symtab);
    a->loop_depth++;

    int last = node->child_count - 1;
    for (int i = 0; i < last; i++) {
        ASTNode *child = node->children[i];
        if (child->kind == AST_VAR_DECL)
            sem_check_var_decl(a, child, 0);
        else
            sem_check_expr(a, child);
    }
    if (last >= 0)
        sem_check_stmt(a, node->children[last]);

    a->loop_depth--;
    sem_scope_exit(&a->symtab);
}

static void check_return(SemAnalyzer *a, ASTNode *node)
{
    /* children = [AST_IDENTIFIER "return", (expr)?] */
    ASTNode *expr = node->child_count > 1 ? node->children[1] : NULL;

    if (sem_type_is_void(&a->current_return)) {
        if (expr) {
            SemType t = sem_check_expr(a, expr);
            (void)t;
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "funcao com retorno 'void' nao pode retornar um valor");
        }
        return;
    }

    if (!expr) {
        char s[SEM_MAX_NAME + 32];
        sem_type_to_string(&a->current_return, s, sizeof(s));
        sem_warning(&a->diags, sem_line(node), sem_col(node),
                    "'return' sem valor numa funcao que devia retornar '%s'", s);
        return;
    }

    SemType t = sem_check_expr(a, expr);
    int line = sem_line(expr), col = sem_col(expr);
    if (line < 0) { line = sem_line(node); col = sem_col(node); }
    sem_check_assign_compat(a, a->current_return, t, expr, line, col, "return");
}

void sem_check_stmt(SemAnalyzer *a, ASTNode *node)
{
    if (!node)
        return;

    switch (node->kind) {
        case AST_BLOCK:
            sem_check_block(a, node, 1);
            break;

        case AST_VAR_DECL:
            sem_check_var_decl(a, node, 0);
            break;

        case AST_EXPR_STMT:
            if (node->child_count > 0)
                sem_check_expr(a, node->children[0]);
            break;

        case AST_IF_STMT:
            if (node->text && strcmp(node->text, "else") == 0) {
                if (node->child_count > 0)
                    sem_check_stmt(a, node->children[0]);
                break;
            }
            if (node->child_count > 0)
                check_condition(a, node->children[0], "if");
            if (node->child_count > 1)
                sem_check_stmt(a, node->children[1]);
            if (node->child_count > 2)
                sem_check_stmt(a, node->children[2]);
            break;

        case AST_WHILE_STMT:
            if (node->child_count > 0)
                check_condition(a, node->children[0], "while");
            a->loop_depth++;
            if (node->child_count > 1)
                sem_check_stmt(a, node->children[1]);
            a->loop_depth--;
            break;

        case AST_DO_WHILE_STMT:
            a->loop_depth++;
            if (node->child_count > 0)
                sem_check_stmt(a, node->children[0]);
            a->loop_depth--;
            if (node->child_count > 1)
                check_condition(a, node->children[1], "do-while");
            break;

        case AST_FOR_STMT:
            check_for(a, node);
            break;

        case AST_SWITCH_STMT:
            check_switch(a, node);
            break;

        case AST_CASE_STMT:
            if (node->child_count > 0) {
                SemType t = sem_check_expr(a, node->children[0]);
                if (t.valid && !sem_type_is_integer(&t))
                    sem_error(&a->diags, sem_line(node->children[0]), sem_col(node->children[0]),
                              "o rotulo 'case' deve ser uma constante inteira");
            }
            for (int i = 1; i < node->child_count; i++) {
                if (node->children[i]->kind == AST_VAR_DECL)
                    sem_check_var_decl(a, node->children[i], 0);
                else
                    sem_check_stmt(a, node->children[i]);
            }
            break;

        case AST_DEFAULT_STMT:
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->kind == AST_VAR_DECL)
                    sem_check_var_decl(a, node->children[i], 0);
                else
                    sem_check_stmt(a, node->children[i]);
            }
            break;

        case AST_BREAK_STMT:
            if (a->loop_depth == 0 && a->switch_depth == 0)
                sem_error(&a->diags, sem_line(node), sem_col(node),
                          "'break' so pode ser usado dentro de um ciclo ou 'switch'");
            break;

        case AST_CONTINUE_STMT:
            if (a->loop_depth == 0)
                sem_error(&a->diags, sem_line(node), sem_col(node),
                          "'continue' so pode ser usado dentro de um ciclo");
            break;

        case AST_RETURN_STMT:
            check_return(a, node);
            break;

        case AST_GENERAL_DECL:
            if (node->child_count > 0)
                sem_type_from_spec(a, node->children[0]); /* regista struct/union local */
            break;

        default:
            sem_check_expr(a, node);
            break;
    }
}
