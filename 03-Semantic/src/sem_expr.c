#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "sem_check.h"

/* ------------------------------------------------------------------ */
/*  Classificacao de operadores (pelo texto do no)                     */
/* ------------------------------------------------------------------ */

static int op_is(const char *op, const char *value)
{
    return op && strcmp(op, value) == 0;
}

static int is_assign_op(const char *op)
{
    return op_is(op, "=")  || op_is(op, "+=") || op_is(op, "-=") ||
           op_is(op, "*=") || op_is(op, "/=") || op_is(op, "%=") ||
           op_is(op, "&=") || op_is(op, "|=") || op_is(op, "^=") ||
           op_is(op, "<<=") || op_is(op, ">>=");
}

static int is_relational_op(const char *op)
{
    return op_is(op, "<") || op_is(op, ">") || op_is(op, "<=") ||
           op_is(op, ">=") || op_is(op, "==") || op_is(op, "!=");
}

static int is_logical_op(const char *op)
{
    return op_is(op, "&&") || op_is(op, "||");
}

static int is_bitwise_op(const char *op)
{
    return op_is(op, "&") || op_is(op, "|") || op_is(op, "^") ||
           op_is(op, "<<") || op_is(op, ">>");
}

/* ------------------------------------------------------------------ */
/*  Literais e lvalues                                                 */
/* ------------------------------------------------------------------ */

static SemType infer_literal(const char *text)
{
    if (!text || !text[0])
        return sem_type_int();
    if (text[0] == '"') {           /* "..." -> char*  */
        SemType t = sem_type_make(TOKEN_CHAR, NULL);
        t.pointer_level = 1;
        return t;
    }
    if (text[0] == '\'')            /* 'a' -> char     */
        return sem_type_make(TOKEN_CHAR, NULL);
    if (strchr(text, '.'))          /* 3.14 -> double  */
        return sem_type_make(TOKEN_DOUBLE, NULL);
    return sem_type_int();          /* 42 -> int       */
}

static int is_zero_literal(ASTNode *node)
{
    if (!node || node->kind != AST_LITERAL || !node->text)
        return 0;
    const char *t = node->text;
    if (t[0] == '"' || t[0] == '\'')
        return 0;
    for (const char *p = t; *p; p++)
        if (*p != '0')
            return 0;
    return t[0] != '\0';
}

int sem_is_lvalue(ASTNode *node)
{
    if (!node)
        return 0;
    switch (node->kind) {
        case AST_IDENTIFIER:
            return 1;
        case AST_SUBSCRIPT_EXPR:
        case AST_MEMBER_EXPR:
            return 1;
        case AST_UNARY_EXPR:
            return node->text && strcmp(node->text, "*") == 0; /* *ptr e lvalue */
        default:
            return 0;
    }
}

/* ------------------------------------------------------------------ */
/*  Compatibilidade de tipos                                           */
/* ------------------------------------------------------------------ */

typedef enum {
    COMPAT_OK,        /* compativel */
    COMPAT_NARROW,    /* aritmetico com possivel perda de precisao (aviso) */
    COMPAT_PTR,       /* ponteiros incompativeis (aviso) */
    COMPAT_BAD        /* incompativel (erro) */
} CompatResult;

/* Tipo opaco: identificador nao resolvido (tipo externo desconhecido). */
static int is_opaque(const SemType *t)
{
    return t->valid && t->base == TOKEN_IDENTIFIER;
}

static CompatResult type_compatible(SemAnalyzer *a, SemType dst, SemType src, ASTNode *src_node)
{
    dst = sem_resolve(a, dst);
    src = sem_resolve(a, src);

    if (!dst.valid || !src.valid)
        return COMPAT_OK;                 /* suprime cascata de erros */
    if (is_opaque(&dst) || is_opaque(&src))
        return COMPAT_OK;                 /* tipo externo desconhecido: leniente */

    int dst_ptr = sem_type_is_pointer(&dst);
    int src_ptr = sem_type_is_pointer(&src);

    if (sem_type_is_arithmetic(&dst) && sem_type_is_arithmetic(&src)) {
        int narrowing = (sem_type_is_floating(&src) && sem_type_is_integer(&dst)) ||
                        (sem_type_rank(&src) > sem_type_rank(&dst));
        return narrowing ? COMPAT_NARROW : COMPAT_OK;
    }

    if (dst_ptr && src_ptr) {
        int dst_void = dst.base == TOKEN_VOID;
        int src_void = src.base == TOKEN_VOID;
        if (dst_void || src_void)
            return COMPAT_OK;             /* void* compativel com qualquer ponteiro */
        if (dst.base == src.base && dst.pointer_level == src.pointer_level)
            return COMPAT_OK;
        return COMPAT_PTR;
    }

    if (dst_ptr && sem_type_is_integer(&src))
        return is_zero_literal(src_node) ? COMPAT_OK : COMPAT_BAD; /* 0 == ponteiro nulo */

    if (dst_ptr || src_ptr)
        return COMPAT_BAD;               /* ponteiro <-> nao-inteiro/nao-ponteiro */

    if (sem_type_is_aggregate(&dst) && sem_type_is_aggregate(&src)) {
        if (dst.base == src.base && strcmp(dst.name, src.name) == 0)
            return COMPAT_OK;
        return COMPAT_BAD;
    }

    if (sem_type_is_void(&dst) || sem_type_is_void(&src))
        return COMPAT_BAD;

    if (sem_type_is_aggregate(&dst) || sem_type_is_aggregate(&src))
        return COMPAT_BAD;

    return COMPAT_OK;
}

/*
 * Verifica a compatibilidade de uma atribuicao/inicializacao/passagem de
 * argumento, emitindo o diagnostico apropriado. @p context descreve o local
 * (ex.: "atribuicao", "inicializacao", "argumento 2 de 'f'").
 */
void sem_check_assign_compat(SemAnalyzer *a, SemType dst, SemType src, ASTNode *src_node,
                             int line, int column, const char *context)
{
    char sdst[SEM_MAX_NAME + 32], ssrc[SEM_MAX_NAME + 32];
    CompatResult r = type_compatible(a, dst, src, src_node);
    if (r == COMPAT_OK)
        return;

    sem_type_to_string(&dst, sdst, sizeof(sdst));
    sem_type_to_string(&src, ssrc, sizeof(ssrc));

    switch (r) {
        case COMPAT_NARROW:
            sem_warning(&a->diags, line, column,
                        "%s: conversao implicita de '%s' para '%s' pode perder informacao",
                        context, ssrc, sdst);
            break;
        case COMPAT_PTR:
            sem_warning(&a->diags, line, column,
                        "%s: tipos de ponteiro incompativeis ('%s' e '%s')",
                        context, ssrc, sdst);
            break;
        case COMPAT_BAD:
            sem_error(&a->diags, line, column,
                      "%s: incompatibilidade de tipos (nao e possivel usar '%s' onde se espera '%s')",
                      context, ssrc, sdst);
            break;
        default:
            break;
    }
}

/* ------------------------------------------------------------------ */
/*  Resultado de operacoes aritmeticas                                 */
/* ------------------------------------------------------------------ */

static SemType arith_result(SemType left, SemType right)
{
    SemType r = sem_type_rank(&left) >= sem_type_rank(&right) ? left : right;
    r.pointer_level = 0;
    r.array_level = 0;
    if (sem_type_rank(&r) < 3)     /* promocao de char/short para int */
        return sem_type_int();
    return r;
}

/* ------------------------------------------------------------------ */
/*  Sub-verificadores por especie de expressao                         */
/* ------------------------------------------------------------------ */

static SemType check_identifier(SemAnalyzer *a, ASTNode *node)
{
    SemSymbol *sym = sem_lookup(&a->symtab, node->text);
    if (!sym) {
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "variavel '%s' nao declarada", node->text);
        return sem_type_invalid();
    }
    sym->is_used = 1;
    return sym->type;
}

static SemType check_assignment(SemAnalyzer *a, ASTNode *node, const char *op)
{
    ASTNode *left = node->children[0];
    ASTNode *right = node->children[1];

    SemType lt = sem_check_expr(a, left);
    SemType rt = sem_check_expr(a, right);

    if (!sem_is_lvalue(left)) {
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "o lado esquerdo de '%s' nao e um lvalue (nao pode receber atribuicao)", op);
        return lt;
    }

    if (strcmp(op, "=") == 0) {
        sem_check_assign_compat(a, lt, rt, right, sem_line(node), sem_col(node), "atribuicao");
    } else {
        /* atribuicoes compostas (+=, &=, ...) exigem operandos escalares/inteiros */
        int bitwise = op_is(op, "&=") || op_is(op, "|=") || op_is(op, "^=") ||
                      op_is(op, "<<=") || op_is(op, ">>=") || op_is(op, "%=");
        if (bitwise) {
            if (lt.valid && rt.valid && (!sem_type_is_integer(&lt) || !sem_type_is_integer(&rt)))
                sem_error(&a->diags, sem_line(node), sem_col(node),
                          "operador '%s' requer operandos inteiros", op);
        } else if (lt.valid && rt.valid && (!sem_type_is_scalar(&lt) || !sem_type_is_scalar(&rt))) {
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%s' requer operandos numericos", op);
        }
    }
    return lt;
}

static SemType check_binary(SemAnalyzer *a, ASTNode *node)
{
    const char *op = node->text;

    if (is_assign_op(op))
        return check_assignment(a, node, op);

    SemType lt = sem_check_expr(a, node->children[0]);
    SemType rt = sem_check_expr(a, node->children[1]);

    if (!lt.valid || !rt.valid)
        return sem_type_invalid();

    if (is_logical_op(op)) {
        if (!sem_type_is_scalar(&lt) || !sem_type_is_scalar(&rt))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador logico '%s' requer operandos escalares", op);
        return sem_type_int();
    }

    if (is_relational_op(op)) {
        if (sem_type_is_aggregate(&lt) || sem_type_is_aggregate(&rt))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%s' nao pode comparar tipos agregados (struct/union)", op);
        else if (sem_type_is_pointer(&lt) != sem_type_is_pointer(&rt) &&
                 !is_zero_literal(node->children[0]) && !is_zero_literal(node->children[1]))
            sem_warning(&a->diags, sem_line(node), sem_col(node),
                        "comparacao '%s' entre ponteiro e inteiro", op);
        return sem_type_int();
    }

    if (is_bitwise_op(op)) {
        if (!sem_type_is_integer(&lt) || !sem_type_is_integer(&rt))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%s' requer operandos inteiros", op);
        return sem_type_int();
    }

    if (op_is(op, "%")) {
        if (!sem_type_is_integer(&lt) || !sem_type_is_integer(&rt))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%%' requer operandos inteiros");
        return sem_type_int();
    }

    /* aritmeticos: +, -, *, / */
    int ptr_l = sem_type_is_pointer(&lt);
    int ptr_r = sem_type_is_pointer(&rt);
    if (ptr_l || ptr_r) {
        if ((op_is(op, "+") || op_is(op, "-")) &&
            ((ptr_l && sem_type_is_integer(&rt)) || (sem_type_is_integer(&lt) && ptr_r)))
            return ptr_l ? lt : rt;             /* aritmetica de ponteiros */
        if (op_is(op, "-") && ptr_l && ptr_r)
            return sem_type_int();              /* diferenca de ponteiros */
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "operador '%s' invalido para operandos do tipo ponteiro", op);
        return sem_type_invalid();
    }

    if (!sem_type_is_arithmetic(&lt) || !sem_type_is_arithmetic(&rt)) {
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "operador '%s' requer operandos numericos", op);
        return sem_type_invalid();
    }
    return arith_result(lt, rt);
}

static SemType check_unary(SemAnalyzer *a, ASTNode *node)
{
    const char *op = node->text;

    if (op_is(op, "sizeof")) {
        if (node->child_count > 0)
            sem_check_expr(a, node->children[0]);
        return sem_type_int();
    }

    if (op_is(op, "cast")) {
        if (node->child_count > 0)
            sem_check_expr(a, node->children[0]);
        if (node->child_count > 1)
            return sem_type_from_spec(a, node->children[1]);
        return sem_type_invalid();
    }

    if (node->child_count == 0)
        return sem_type_invalid();

    SemType t = sem_check_expr(a, node->children[0]);
    ASTNode *operand = node->children[0];

    if (op_is(op, "&")) {
        if (!sem_is_lvalue(operand))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '&' requer um lvalue");
        if (t.valid) t.pointer_level++;
        return t;
    }

    if (op_is(op, "*")) {
        if (t.valid && !sem_type_is_pointer(&t)) {
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "nao e possivel desreferenciar um valor que nao e ponteiro");
            return sem_type_invalid();
        }
        if (t.array_level > 0) t.array_level--;
        else if (t.pointer_level > 0) t.pointer_level--;
        return t;
    }

    if (op_is(op, "!"))
        return sem_type_int();

    if (op_is(op, "~")) {
        if (t.valid && !sem_type_is_integer(&t))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '~' requer operando inteiro");
        return sem_type_int();
    }

    if (op_is(op, "++") || op_is(op, "--")) {
        if (!sem_is_lvalue(operand))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%s' requer um lvalue", op);
        else if (t.valid && !sem_type_is_scalar(&t))
            sem_error(&a->diags, sem_line(node), sem_col(node),
                      "operador '%s' requer operando numerico ou ponteiro", op);
        return t;
    }

    /* '-' e '+' unarios */
    if (t.valid && !sem_type_is_arithmetic(&t)) {
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "operador unario '%s' requer operando numerico", op);
        return sem_type_invalid();
    }
    return t;
}

static SemType check_call(SemAnalyzer *a, ASTNode *node)
{
    ASTNode *callee = node->child_count > 0 ? node->children[0] : NULL;
    ASTNode *args = node->child_count > 1 ? node->children[1] : NULL;
    int argc = args ? args->child_count : 0;

    if (!callee || callee->kind != AST_IDENTIFIER) {
        if (callee) sem_check_expr(a, callee);
        for (int i = 0; i < argc; i++)
            sem_check_expr(a, args->children[i]);
        return sem_type_int();
    }

    SemSymbol *fn = sem_lookup(&a->symtab, callee->text);

    if (!fn) {
        sem_warning(&a->diags, sem_line(callee), sem_col(callee),
                    "funcao '%s' nao declarada (assumida como externa)", callee->text);
        for (int i = 0; i < argc; i++)
            sem_check_expr(a, args->children[i]);
        return sem_type_int();
    }

    fn->is_used = 1;

    if (fn->kind != SYM_FUNCTION) {
        sem_error(&a->diags, sem_line(callee), sem_col(callee),
                  "'%s' nao e uma funcao", callee->text);
        for (int i = 0; i < argc; i++)
            sem_check_expr(a, args->children[i]);
        return sem_type_invalid();
    }

    /* Tipos dos argumentos (sempre verificados para apanhar erros internos). */
    SemType arg_types[SEM_MAX_PARAMS];
    for (int i = 0; i < argc; i++) {
        SemType t = sem_check_expr(a, args->children[i]);
        if (i < SEM_MAX_PARAMS)
            arg_types[i] = t;
    }

    if (!fn->is_variadic && argc != fn->param_count) {
        sem_error(&a->diags, sem_line(callee), sem_col(callee),
                  "funcao '%s' espera %d argumento(s) mas recebeu %d",
                  fn->name, fn->param_count, argc);
    } else if (!fn->is_variadic) {
        for (int i = 0; i < argc && i < fn->param_count && i < SEM_MAX_PARAMS; i++) {
            char ctx[SEM_MAX_NAME + 32];
            snprintf(ctx, sizeof(ctx), "argumento %d de '%s'", i + 1, fn->name);
            int line = sem_line(args->children[i]);
            int col = sem_col(args->children[i]);
            if (line < 0) { line = sem_line(callee); col = sem_col(callee); }
            sem_check_assign_compat(a, fn->param_types[i], arg_types[i], args->children[i], line, col, ctx);
        }
    }

    return fn->type;
}

static SemType check_subscript(SemAnalyzer *a, ASTNode *node)
{
    SemType arr = sem_check_expr(a, node->children[0]);
    SemType idx = sem_check_expr(a, node->children[1]);

    /* Um array/ponteiro pode ser indexado. Tipos agregados ou void nao.
       Um escalar aritmetico e tolerado porque o parser descarta os
       colchetes de arrays vazios em parametros (ex.: 'int v[]'). */
    if (arr.valid && !sem_type_is_pointer(&arr) &&
        (sem_type_is_aggregate(&arr) || sem_type_is_void(&arr))) {
        char s[SEM_MAX_NAME + 32];
        sem_type_to_string(&arr, s, sizeof(s));
        sem_error(&a->diags, sem_line(node->children[0]), sem_col(node->children[0]),
                  "indexacao ('[]') aplicada a tipo nao indexavel '%s'", s);
        return sem_type_invalid();
    }
    if (idx.valid && !sem_type_is_integer(&idx))
        sem_error(&a->diags, sem_line(node->children[1]), sem_col(node->children[1]),
                  "o indice de um array deve ser um inteiro");

    if (arr.array_level > 0) arr.array_level--;
    else if (arr.pointer_level > 0) arr.pointer_level--;
    return arr;
}

static SemType check_member(SemAnalyzer *a, ASTNode *node)
{
    int arrow = node->text && strcmp(node->text, "->") == 0;
    SemType obj = sem_resolve(a, sem_check_expr(a, node->children[0]));
    ASTNode *member_node = node->children[1];
    const char *member = member_node ? member_node->text : NULL;

    if (!obj.valid)
        return sem_type_invalid();

    if (arrow) {
        if (obj.pointer_level != 1 || (obj.base != TOKEN_STRUCT && obj.base != TOKEN_UNION)) {
            if (!is_opaque(&obj))
                sem_error(&a->diags, sem_line(node), sem_col(node),
                          "operador '->' requer ponteiro para struct/union");
            return sem_type_invalid();
        }
        obj.pointer_level = 0;
    } else {
        if (!sem_type_is_aggregate(&obj)) {
            if (!is_opaque(&obj))
                sem_error(&a->diags, sem_line(node), sem_col(node),
                          "operador '.' requer um valor struct/union");
            return sem_type_invalid();
        }
    }

    SemTag *tag = sem_tag_find(a, obj.name);
    if (!tag || !tag->defined)
        return sem_type_invalid();     /* struct externa/incompleta: leniente */

    for (int i = 0; i < tag->field_count; i++) {
        if (member && strcmp(tag->field_names[i], member) == 0)
            return tag->field_types[i];
    }

    sem_error(&a->diags, sem_line(node), sem_col(node),
              "'%s' nao e um membro de '%s %s'", member ? member : "?",
              tag->is_union ? "union" : "struct", tag->name);
    return sem_type_invalid();
}

static SemType check_conditional(SemAnalyzer *a, ASTNode *node)
{
    SemType cond = sem_check_expr(a, node->children[0]);
    SemType a_t = sem_check_expr(a, node->children[1]);
    SemType b_t = sem_check_expr(a, node->children[2]);

    if (cond.valid && !sem_type_is_scalar(&cond))
        sem_error(&a->diags, sem_line(node), sem_col(node),
                  "a condicao do operador '?:' deve ser escalar");

    if (sem_type_is_arithmetic(&a_t) && sem_type_is_arithmetic(&b_t))
        return arith_result(a_t, b_t);
    return a_t.valid ? a_t : b_t;
}

/* ------------------------------------------------------------------ */
/*  Despacho principal                                                 */
/* ------------------------------------------------------------------ */

SemType sem_check_expr(SemAnalyzer *a, ASTNode *node)
{
    if (!node)
        return sem_type_invalid();

    switch (node->kind) {
        case AST_IDENTIFIER:        return check_identifier(a, node);
        case AST_LITERAL:           return infer_literal(node->text);
        case AST_BINARY_EXPR:       return check_binary(a, node);
        case AST_UNARY_EXPR:        return check_unary(a, node);
        case AST_CALL_EXPR:         return check_call(a, node);
        case AST_SUBSCRIPT_EXPR:    return check_subscript(a, node);
        case AST_MEMBER_EXPR:       return check_member(a, node);
        case AST_CONDITIONAL_EXPR:  return check_conditional(a, node);
        case AST_ERROR:             return sem_type_invalid();
        default:
            /* No inesperado: percorrer filhos para nao perder verificacoes. */
            for (int i = 0; i < node->child_count; i++)
                sem_check_expr(a, node->children[i]);
            return sem_type_invalid();
    }
}
