#ifndef SEM_TYPES_H
#define SEM_TYPES_H

#include "token.h"

/**
 * @file sem_types.h
 * @brief Sistema de tipos do analisador semantico.
 *
 * Um @ref SemType descreve o tipo de uma variavel, parametro, campo ou
 * do resultado de uma expressao. E composto por um tipo base (token de
 * palavra reservada), um nivel de indireccao (ponteiros) e um nivel de
 * array. Tipos definidos pelo utilizador (typedef/struct/union) guardam
 * ainda o nome da etiqueta em @ref SemType::name.
 */

/** @brief Tamanho maximo de um nome de tipo/etiqueta. */
#define SEM_MAX_NAME 128

/**
 * @brief Representacao de um tipo semantico.
 */
typedef struct {
    int  base;           /**< Token do tipo base (TOKEN_INT, TOKEN_FLOAT, TOKEN_STRUCT, TOKEN_IDENTIFIER, TOKEN_VOID, ...). */
    int  pointer_level;  /**< Numero de niveis de ponteiro (`*`). */
    int  array_level;    /**< Numero de dimensoes de array. */
    char name[SEM_MAX_NAME]; /**< Nome da etiqueta (struct/union) ou do typedef. */
    int  valid;          /**< 0 => tipo desconhecido/erro (suprime erros em cascata). */
} SemType;

/** @brief Constroi um tipo base simples. */
SemType sem_type_make(int base, const char *name);
/** @brief Tipo invalido (usado para suprimir erros em cascata). */
SemType sem_type_invalid(void);
/** @brief Atalho para o tipo `int`. */
SemType sem_type_int(void);
/** @brief Atalho para o tipo `void`. */
SemType sem_type_void(void);

/** @brief Verdadeiro se o tipo e aritmetico (inteiro ou virgula flutuante) escalar. */
int sem_type_is_arithmetic(const SemType *t);
/** @brief Verdadeiro se o tipo e inteiro (char/short/int/long) sem indireccao. */
int sem_type_is_integer(const SemType *t);
/** @brief Verdadeiro se o tipo e de virgula flutuante (float/double). */
int sem_type_is_floating(const SemType *t);
/** @brief Verdadeiro se o tipo e ponteiro ou array. */
int sem_type_is_pointer(const SemType *t);
/** @brief Verdadeiro se o tipo e escalar (aritmetico ou ponteiro). */
int sem_type_is_scalar(const SemType *t);
/** @brief Verdadeiro se o tipo e `void` sem indireccao. */
int sem_type_is_void(const SemType *t);
/** @brief Verdadeiro se o tipo e agregado (struct/union) sem indireccao. */
int sem_type_is_aggregate(const SemType *t);

/** @brief Ordena os tipos aritmeticos para as conversoes usuais (char<...<double). */
int  sem_type_rank(const SemType *t);
/** @brief Escreve uma representacao textual do tipo em @p buf. */
void sem_type_to_string(const SemType *t, char *buf, int size);

#endif /* SEM_TYPES_H */
