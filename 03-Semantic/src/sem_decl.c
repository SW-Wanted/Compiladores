#include <string.h>

#include "sem_check.h"

/**
 * @brief Preenche os tipos dos parametros de uma funcao a partir da AST.
 */
static void fill_parameters(SemAnalyzer *a, ASTNode *param_list, SemSymbol *sym)
{
    sym->param_count = 0;
    if (!param_list)
        return;
    for (int i = 0; i < param_list->child_count; i++) {
        ASTNode *p = param_list->children[i];
        if (p->kind != AST_PARAM || p->child_count == 0)
            continue;
        SemType base = sem_type_from_spec(a, p->children[0]);
        SemType t = (p->child_count > 1) ? sem_type_from_declarator(base, p->children[1]) : base;
        if (sym->param_count < SEM_MAX_PARAMS)
            sym->param_types[sym->param_count++] = t;
    }
}

/**
 * @brief Regista a assinatura de uma funcao (deteccao de redefinicao).
 *
 * Um prototipo seguido de definicao actualiza o mesmo simbolo; duas definicoes
 * geram erro. Builtins podem ser sobrepostos por definicoes do utilizador.
 */
static void register_function(SemAnalyzer *a, ASTNode *func)
{
    if (func->child_count < 3)
        return;

    ASTNode *type_spec = func->children[0];
    ASTNode *declarator = func->children[1];
    ASTNode *param_list = func->children[2];
    int has_body = func->child_count > 3 && func->children[3] &&
                   func->children[3]->kind == AST_BLOCK;

    const char *name = sem_declarator_name(declarator);
    if (!name)
        return;

    SemType base = sem_type_from_spec(a, type_spec);
    SemType ret = sem_type_from_declarator(base, declarator);

    SemSymbol symbol;
    memset(&symbol, 0, sizeof(symbol));
    strncpy(symbol.name, name, SEM_MAX_NAME - 1);
    symbol.kind = SYM_FUNCTION;
    symbol.type = ret;
    symbol.is_defined = has_body;
    symbol.is_variadic = 0;
    sem_node_position(declarator, &symbol.line, &symbol.column);
    fill_parameters(a, param_list, &symbol);

    SemSymbol *existing = sem_lookup_current(&a->symtab, name);
    if (existing) {
        if (existing->kind != SYM_FUNCTION) {
            sem_error(&a->diags, symbol.line, symbol.column,
                      "'%s' ja foi declarado como um identificador diferente", name);
            return;
        }
        if (existing->is_defined && has_body) {
            sem_error(&a->diags, symbol.line, symbol.column,
                      "redefinicao da funcao '%s' (definida na linha %d)",
                      name, existing->line);
            return;
        }
        int was_defined = existing->is_defined;
        int builtin = existing->is_variadic;
        *existing = symbol;
        existing->active = 1;
        existing->scope_level = 0;
        existing->is_defined = was_defined || has_body;
        if (builtin && !has_body)
            existing->is_variadic = 1;
        return;
    }

    int duplicate = 0;
    sem_declare(&a->symtab, &symbol, &duplicate);
}

/**
 * @brief Regista um typedef, associando o nome ao tipo subjacente.
 */
static void register_typedef(SemAnalyzer *a, ASTNode *node)
{
    if (node->child_count < 2)
        return;
    SemType base = sem_type_from_spec(a, node->children[0]);
    SemType type = sem_type_from_declarator(base, node->children[1]);
    const char *name = sem_declarator_name(node->children[1]);
    if (!name)
        return;

    SemSymbol symbol;
    memset(&symbol, 0, sizeof(symbol));
    strncpy(symbol.name, name, SEM_MAX_NAME - 1);
    symbol.kind = SYM_TYPEDEF;
    symbol.type = type;
    sem_node_position(node->children[1], &symbol.line, &symbol.column);

    int duplicate = 0;
    SemSymbol *slot = sem_declare(&a->symtab, &symbol, &duplicate);
    if (duplicate && slot)
        sem_error(&a->diags, symbol.line, symbol.column,
                  "redeclaracao do tipo '%s'", name);
}

/**
 * @brief Regista uma macro `#define` como constante (evita 'nao declarada').
 */
static void register_define(SemAnalyzer *a, ASTNode *node)
{
    if (node->child_count == 0 || !node->children[0]->text)
        return;
    const char *name = node->children[0]->text;

    SemType type = sem_type_int();
    if (node->child_count > 1 && node->children[1]->text) {
        const char *v = node->children[1]->text;
        if (v[0] == '"') { type = sem_type_make(TOKEN_CHAR, NULL); type.pointer_level = 1; }
        else if (strchr(v, '.')) type = sem_type_make(TOKEN_DOUBLE, NULL);
    }

    if (sem_lookup_current(&a->symtab, name))
        return;

    SemSymbol symbol;
    memset(&symbol, 0, sizeof(symbol));
    strncpy(symbol.name, name, SEM_MAX_NAME - 1);
    symbol.kind = SYM_ENUM_CONST;
    symbol.type = type;
    symbol.is_initialized = 1;
    sem_node_position(node->children[0], &symbol.line, &symbol.column);

    int duplicate = 0;
    sem_declare(&a->symtab, &symbol, &duplicate);
}

/**
 * @brief Analisa o corpo de uma funcao (parametros + bloco no mesmo escopo).
 */
static void analyze_function(SemAnalyzer *a, ASTNode *func)
{
    if (func->child_count < 3)
        return;
    ASTNode *type_spec = func->children[0];
    ASTNode *declarator = func->children[1];
    ASTNode *param_list = func->children[2];
    ASTNode *body = (func->child_count > 3 && func->children[3]->kind == AST_BLOCK)
                        ? func->children[3] : NULL;
    if (!body)
        return;

    SemType base = sem_type_from_spec(a, type_spec);
    a->current_return = sem_type_from_declarator(base, declarator);
    a->in_function = 1;
    a->loop_depth = 0;
    a->switch_depth = 0;

    sem_scope_enter(&a->symtab);

    for (int i = 0; i < param_list->child_count; i++) {
        ASTNode *p = param_list->children[i];
        if (p->kind != AST_PARAM || p->child_count < 2)
            continue;
        SemType pbase = sem_type_from_spec(a, p->children[0]);
        SemType ptype = sem_type_from_declarator(pbase, p->children[1]);
        const char *pname = sem_declarator_name(p->children[1]);
        if (!pname)
            continue;

        SemSymbol psym;
        memset(&psym, 0, sizeof(psym));
        strncpy(psym.name, pname, SEM_MAX_NAME - 1);
        psym.kind = SYM_PARAM;
        psym.type = ptype;
        psym.is_initialized = 1;
        sem_node_position(p->children[1], &psym.line, &psym.column);

        int duplicate = 0;
        SemSymbol *slot = sem_declare(&a->symtab, &psym, &duplicate);
        if (duplicate && slot)
            sem_error(&a->diags, psym.line, psym.column,
                      "parametro '%s' declarado mais de uma vez", pname);
    }

    sem_check_block(a, body, 0);

    sem_scope_exit(&a->symtab);
    a->in_function = 0;
}

/**
 * @brief Devolve a lista de declaracoes globais do programa.
 */
static ASTNode *global_list(ASTNode *root)
{
    if (root && root->kind == AST_PROGRAM && root->child_count > 0 &&
        root->children[0]->kind == AST_DECLARATOR_LIST)
        return root->children[0];
    return root;
}

/**
 * @brief 1a passagem: regista funcoes, typedefs, etiquetas e macros globais.
 *
 * Permite referencias antecipadas e recursao mutua entre funcoes.
 */
static void collect_globals(SemAnalyzer *a, ASTNode *list)
{
    for (int i = 0; i < list->child_count; i++) {
        ASTNode *decl = list->children[i];
        switch (decl->kind) {
            case AST_FUNC_DECL:        register_function(a, decl); break;
            case AST_TYPEDEF_DECL:     register_typedef(a, decl); break;
            case AST_DIRECTIVE_DEFINE: register_define(a, decl); break;
            case AST_GENERAL_DECL:
                if (decl->child_count > 0)
                    sem_type_from_spec(a, decl->children[0]);
                break;
            default:
                break;
        }
    }
}

/**
 * @brief 2a passagem: verifica corpos de funcoes e variaveis globais em ordem.
 */
static void check_globals(SemAnalyzer *a, ASTNode *list)
{
    for (int i = 0; i < list->child_count; i++) {
        ASTNode *decl = list->children[i];
        switch (decl->kind) {
            case AST_FUNC_DECL:  analyze_function(a, decl); break;
            case AST_VAR_DECL:   sem_check_var_decl(a, decl, 1); break;
            default:             break;
        }
    }
}

/**
 * @brief Executa a analise semantica sobre a arvore e devolve o numero de erros.
 */
int sem_analyze(SemAnalyzer *a, ASTNode *root)
{
    if (!root)
        return 0;
    ASTNode *list = global_list(root);
    collect_globals(a, list);
    check_globals(a, list);
    return a->diags.error_count;
}
