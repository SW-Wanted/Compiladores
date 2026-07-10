#include <string.h>
#include <stdio.h>

#include "sem_types.h"

/**
 * @brief Constroi um tipo base simples (opcionalmente com nome de etiqueta).
 */
SemType sem_type_make(int base, const char *name)
{
    SemType t;
    memset(&t, 0, sizeof(t));
    t.base = base;
    t.pointer_level = 0;
    t.array_level = 0;
    t.valid = 1;
    if (name)
        strncpy(t.name, name, SEM_MAX_NAME - 1);
    return t;
}

/**
 * @brief Devolve um tipo invalido, usado para suprimir erros em cascata.
 */
SemType sem_type_invalid(void)
{
    SemType t;
    memset(&t, 0, sizeof(t));
    t.base = TOKEN_UNKNOWN;
    t.valid = 0;
    return t;
}

/**
 * @brief Atalho para o tipo `int`.
 */
SemType sem_type_int(void)
{
    return sem_type_make(TOKEN_INT, NULL);
}

/**
 * @brief Atalho para o tipo `void`.
 */
SemType sem_type_void(void)
{
    return sem_type_make(TOKEN_VOID, NULL);
}

/**
 * @brief Verdadeiro se o tipo e inteiro (char/short/int/long) sem indireccao.
 */
int sem_type_is_integer(const SemType *t)
{
    if (!t->valid || t->pointer_level > 0 || t->array_level > 0)
        return 0;
    switch (t->base) {
        case TOKEN_CHAR:
        case TOKEN_SHORT:
        case TOKEN_INT:
        case TOKEN_LONG:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Verdadeiro se o tipo e de virgula flutuante (float/double).
 */
int sem_type_is_floating(const SemType *t)
{
    if (!t->valid || t->pointer_level > 0 || t->array_level > 0)
        return 0;
    return t->base == TOKEN_FLOAT || t->base == TOKEN_DOUBLE;
}

/**
 * @brief Verdadeiro se o tipo e aritmetico (inteiro ou virgula flutuante).
 */
int sem_type_is_arithmetic(const SemType *t)
{
    return sem_type_is_integer(t) || sem_type_is_floating(t);
}

/**
 * @brief Verdadeiro se o tipo e ponteiro ou array.
 */
int sem_type_is_pointer(const SemType *t)
{
    return t->valid && (t->pointer_level > 0 || t->array_level > 0);
}

/**
 * @brief Verdadeiro se o tipo e escalar (aritmetico ou ponteiro).
 */
int sem_type_is_scalar(const SemType *t)
{
    return sem_type_is_arithmetic(t) || sem_type_is_pointer(t);
}

/**
 * @brief Verdadeiro se o tipo e `void` sem indireccao.
 */
int sem_type_is_void(const SemType *t)
{
    return t->valid && t->base == TOKEN_VOID && t->pointer_level == 0 && t->array_level == 0;
}

/**
 * @brief Verdadeiro se o tipo e agregado (struct/union) sem indireccao.
 */
int sem_type_is_aggregate(const SemType *t)
{
    if (!t->valid || t->pointer_level > 0 || t->array_level > 0)
        return 0;
    return t->base == TOKEN_STRUCT || t->base == TOKEN_UNION;
}

/**
 * @brief Ordena os tipos aritmeticos para as conversoes usuais (char < ... < double).
 */
int sem_type_rank(const SemType *t)
{
    switch (t->base) {
        case TOKEN_CHAR:   return 1;
        case TOKEN_SHORT:  return 2;
        case TOKEN_INT:    return 3;
        case TOKEN_LONG:   return 4;
        case TOKEN_FLOAT:  return 5;
        case TOKEN_DOUBLE: return 6;
        default:           return 3;
    }
}

/**
 * @brief Nome textual do tipo base (NULL se for typedef/desconhecido).
 */
static const char *base_name(int base)
{
    switch (base) {
        case TOKEN_INT:    return "int";
        case TOKEN_FLOAT:  return "float";
        case TOKEN_CHAR:   return "char";
        case TOKEN_VOID:   return "void";
        case TOKEN_DOUBLE: return "double";
        case TOKEN_LONG:   return "long";
        case TOKEN_SHORT:  return "short";
        case TOKEN_STRUCT: return "struct";
        case TOKEN_UNION:  return "union";
        default:           return NULL;
    }
}

/**
 * @brief Escreve uma representacao textual do tipo em @p buf (ex.: "int*", "struct Ponto").
 */
void sem_type_to_string(const SemType *t, char *buf, int size)
{
    char tmp[SEM_MAX_NAME + 32];
    const char *base = base_name(t->base);

    if (!t->valid) {
        snprintf(buf, size, "<desconhecido>");
        return;
    }

    if ((t->base == TOKEN_STRUCT || t->base == TOKEN_UNION) && t->name[0])
        snprintf(tmp, sizeof(tmp), "%s %s", base, t->name);
    else if (base)
        snprintf(tmp, sizeof(tmp), "%s", base);
    else if (t->name[0])
        snprintf(tmp, sizeof(tmp), "%s", t->name);
    else
        snprintf(tmp, sizeof(tmp), "int");

    int pos = (int)strlen(tmp);
    for (int i = 0; i < t->pointer_level && pos < (int)sizeof(tmp) - 2; i++)
        tmp[pos++] = '*';
    tmp[pos] = '\0';

    if (t->array_level > 0 && pos < (int)sizeof(tmp) - 3) {
        for (int i = 0; i < t->array_level && pos < (int)sizeof(tmp) - 3; i++) {
            tmp[pos++] = '[';
            tmp[pos++] = ']';
        }
        tmp[pos] = '\0';
    }

    snprintf(buf, size, "%s", tmp);
}
