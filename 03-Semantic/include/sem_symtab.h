#ifndef SEM_SYMTAB_H
#define SEM_SYMTAB_H

#include "sem_types.h"
#include "scope_table.h"   /* reutiliza a enumeracao SymbolKind da fase do parser */

/**
 * @file sem_symtab.h
 * @brief Tabela de simbolos com pilha de escopos do analisador semantico.
 *
 * Ao contrario da tabela plana produzida pelo parser, esta tabela mantem
 * uma verdadeira pilha de escopos (`sem_scope_enter`/`sem_scope_exit`) que
 * permite verificar redeclaracoes no mesmo escopo e resolver identificadores
 * do escopo mais interno para o mais externo.
 */

/** @brief Capacidade maxima de simbolos. */
#define SEM_MAX_SYMBOLS 2048
/** @brief Profundidade maxima da pilha de escopos. */
#define SEM_MAX_DEPTH   128
/** @brief Numero maximo de parametros registados por funcao. */
#define SEM_MAX_PARAMS  32

/**
 * @brief Entrada da tabela de simbolos semantica.
 */
typedef struct {
    char       name[SEM_MAX_NAME];          /**< Nome do identificador. */
    SymbolKind kind;                        /**< Especie (variavel, funcao, parametro, typedef, ...). */
    SemType    type;                        /**< Tipo (para funcao: tipo de retorno). */
    int        scope_level;                 /**< Nivel de escopo onde foi declarado. */
    int        line;                        /**< Linha da declaracao. */
    int        column;                      /**< Coluna da declaracao. */
    int        param_count;                 /**< Numero de parametros (funcoes). */
    SemType    param_types[SEM_MAX_PARAMS]; /**< Tipos dos parametros (funcoes). */
    int        is_variadic;                 /**< Funcao variadica/externa sem verificacao de argumentos. */
    int        is_defined;                  /**< Funcao com corpo (nao apenas prototipo). */
    int        is_initialized;              /**< Variavel com inicializador. */
    int        is_used;                     /**< Identificador referenciado ao menos uma vez. */
    int        active;                      /**< Ainda visivel (0 apos sair do escopo). */
} SemSymbol;

/**
 * @brief Tabela de simbolos com pilha de escopos.
 */
typedef struct {
    SemSymbol symbols[SEM_MAX_SYMBOLS];  /**< Simbolos por ordem de declaracao. */
    int       count;                     /**< Numero de simbolos. */
    int       scope_start[SEM_MAX_DEPTH];/**< Indice do primeiro simbolo de cada escopo. */
    int       depth;                     /**< Profundidade actual (0 = global). */
    int       level;                     /**< Nivel de escopo actual. */
} SemSymTab;

/** @brief Inicializa a tabela e abre o escopo global. */
void       sem_symtab_init(SemSymTab *table);
/** @brief Abre um novo escopo. */
void       sem_scope_enter(SemSymTab *table);
/** @brief Fecha o escopo actual, desactivando os seus simbolos. */
void       sem_scope_exit(SemSymTab *table);
/**
 * @brief Declara um simbolo no escopo actual.
 * @param duplicate Recebe 1 se ja existia um simbolo com o mesmo nome no escopo.
 * @return Ponteiro para o simbolo inserido (ou existente, se duplicado).
 */
SemSymbol *sem_declare(SemSymTab *table, const SemSymbol *symbol, int *duplicate);
/** @brief Procura um simbolo activo do escopo mais interno para o mais externo. */
SemSymbol *sem_lookup(SemSymTab *table, const char *name);
/** @brief Procura um simbolo activo apenas no escopo actual. */
SemSymbol *sem_lookup_current(SemSymTab *table, const char *name);

#endif /* SEM_SYMTAB_H */
