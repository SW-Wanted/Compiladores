#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "sem_symtab.h"
#include "sem_error.h"

/**
 * @file semantic.h
 * @brief Ponto de entrada do analisador semantico (fase 03).
 *
 * O analisador recebe a arvore sintactica produzida pelo parser (fase 02) e,
 * apoiado numa tabela de simbolos com escopos, verifica as regras semanticas
 * da linguagem: declaracao previa, unicidade no escopo, compatibilidade de
 * tipos em atribuicoes e operacoes, argumentos de funcoes, condicoes de
 * estruturas de controlo, uso correcto de `break`/`continue`/`return`, etc.
 */

/** @brief Numero maximo de etiquetas struct/union registadas. */
#define SEM_MAX_TAGS   128
/** @brief Numero maximo de campos por struct/union. */
#define SEM_MAX_FIELDS 64

/**
 * @brief Definicao de uma etiqueta de struct ou union.
 */
typedef struct {
    char    name[SEM_MAX_NAME];                  /**< Nome da etiqueta (vazio se anonima). */
    int     is_union;                            /**< 1 se for union. */
    int     defined;                             /**< 1 se os campos ja foram registados. */
    int     field_count;                         /**< Numero de campos. */
    char    field_names[SEM_MAX_FIELDS][SEM_MAX_NAME]; /**< Nomes dos campos. */
    SemType field_types[SEM_MAX_FIELDS];         /**< Tipos dos campos. */
} SemTag;

/**
 * @brief Estado global do analisador semantico.
 */
typedef struct {
    SemSymTab   symtab;              /**< Tabela de simbolos com escopos. */
    SemDiagList diags;               /**< Diagnosticos acumulados. */
    SemTag      tags[SEM_MAX_TAGS];  /**< Definicoes de struct/union. */
    int         tag_count;           /**< Numero de etiquetas registadas. */
    int         anon_counter;        /**< Contador para etiquetas anonimas. */
    SemType     current_return;      /**< Tipo de retorno da funcao em analise. */
    int         in_function;         /**< 1 enquanto se analisa o corpo de uma funcao. */
    int         loop_depth;          /**< Profundidade de ciclos (for/while/do). */
    int         switch_depth;        /**< Profundidade de switch. */
} SemAnalyzer;

/** @brief Inicializa o analisador e regista os builtins. */
void sem_analyzer_init(SemAnalyzer *analyzer);
/**
 * @brief Executa a analise semantica sobre a arvore @p root.
 * @return Numero de erros encontrados.
 */
int  sem_analyze(SemAnalyzer *analyzer, ASTNode *root);
/** @brief Imprime a tabela de diagnosticos. */
void sem_print_diagnostics(const SemAnalyzer *analyzer);
/** @brief Imprime a tabela de simbolos semantica. */
void sem_print_symbols(const SemAnalyzer *analyzer);

#endif /* SEMANTIC_H */
