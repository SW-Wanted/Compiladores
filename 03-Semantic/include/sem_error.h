#ifndef SEM_ERROR_H
#define SEM_ERROR_H

/**
 * @file sem_error.h
 * @brief Coleccao de diagnosticos (erros e avisos) do analisador semantico.
 */

/** @brief Capacidade maxima de diagnosticos. */
#define SEM_MAX_DIAGNOSTICS 512
/** @brief Tamanho maximo de uma mensagem de diagnostico. */
#define SEM_MAX_MESSAGE 256

/**
 * @brief Severidade de um diagnostico.
 */
typedef enum {
    SEM_SEV_ERROR,   /**< Erro semantico (invalida o programa). */
    SEM_SEV_WARNING  /**< Aviso (programa aceite, mas suspeito). */
} SemSeverity;

/**
 * @brief Um diagnostico semantico.
 */
typedef struct {
    SemSeverity severity;                 /**< Severidade. */
    char        message[SEM_MAX_MESSAGE]; /**< Mensagem descritiva. */
    int         line;                     /**< Linha associada. */
    int         column;                   /**< Coluna associada. */
} SemDiagnostic;

/**
 * @brief Lista de diagnosticos acumulados durante a analise.
 */
typedef struct {
    SemDiagnostic items[SEM_MAX_DIAGNOSTICS]; /**< Diagnosticos por ordem de emissao. */
    int count;         /**< Total de diagnosticos. */
    int error_count;   /**< Numero de erros. */
    int warning_count; /**< Numero de avisos. */
} SemDiagList;

/** @brief Inicializa a lista de diagnosticos. */
void sem_diag_init(SemDiagList *list);
/** @brief Regista um erro formatado. */
void sem_error(SemDiagList *list, int line, int column, const char *fmt, ...);
/** @brief Regista um aviso formatado. */
void sem_warning(SemDiagList *list, int line, int column, const char *fmt, ...);

#endif /* SEM_ERROR_H */
