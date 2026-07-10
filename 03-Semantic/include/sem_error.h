#ifndef SEM_ERROR_H
#define SEM_ERROR_H

#define SEM_MAX_DIAGNOSTICS 512
#define SEM_MAX_MESSAGE 256

typedef enum {
    SEM_SEV_ERROR,
    SEM_SEV_WARNING
} SemSeverity;

typedef struct {
    SemSeverity severity;
    char        message[SEM_MAX_MESSAGE];
    int         line;
    int         column;
} SemDiagnostic;

typedef struct {
    SemDiagnostic items[SEM_MAX_DIAGNOSTICS];
    int count;
    int error_count;
    int warning_count;
} SemDiagList;

void sem_diag_init(SemDiagList *list);
void sem_error(SemDiagList *list, int line, int column, const char *fmt, ...);
void sem_warning(SemDiagList *list, int line, int column, const char *fmt, ...);

#endif /* SEM_ERROR_H */
