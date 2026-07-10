#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "sem_error.h"

void sem_diag_init(SemDiagList *list)
{
    list->count = 0;
    list->error_count = 0;
    list->warning_count = 0;
}

static void diag_add(SemDiagList *list, SemSeverity severity, int line, int column,
                     const char *fmt, va_list args)
{
    if (list->count >= SEM_MAX_DIAGNOSTICS)
        return;

    SemDiagnostic *diag = &list->items[list->count++];
    diag->severity = severity;
    diag->line = line;
    diag->column = column;
    vsnprintf(diag->message, sizeof(diag->message), fmt, args);

    if (severity == SEM_SEV_ERROR)
        list->error_count++;
    else
        list->warning_count++;
}

void sem_error(SemDiagList *list, int line, int column, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    diag_add(list, SEM_SEV_ERROR, line, column, fmt, args);
    va_end(args);
}

void sem_warning(SemDiagList *list, int line, int column, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    diag_add(list, SEM_SEV_WARNING, line, column, fmt, args);
    va_end(args);
}
