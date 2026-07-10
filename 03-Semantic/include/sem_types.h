#ifndef SEM_TYPES_H
#define SEM_TYPES_H

#include "token.h"

#define SEM_MAX_NAME 128

typedef struct {
    int  base;
    int  pointer_level;
    int  array_level;
    char name[SEM_MAX_NAME];
    int  valid;
} SemType;

SemType sem_type_make(int base, const char *name);
SemType sem_type_invalid(void);
SemType sem_type_int(void);
SemType sem_type_void(void);

int sem_type_is_arithmetic(const SemType *t);
int sem_type_is_integer(const SemType *t);
int sem_type_is_floating(const SemType *t);
int sem_type_is_pointer(const SemType *t);
int sem_type_is_scalar(const SemType *t);
int sem_type_is_void(const SemType *t);
int sem_type_is_aggregate(const SemType *t);

int  sem_type_rank(const SemType *t);
void sem_type_to_string(const SemType *t, char *buf, int size);

#endif /* SEM_TYPES_H */
