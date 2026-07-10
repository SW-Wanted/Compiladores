#ifndef SEM_CHECK_H
#define SEM_CHECK_H

#include "semantic.h"

SemType sem_type_from_spec(SemAnalyzer *analyzer, ASTNode *type_spec);
SemType sem_type_from_declarator(SemType base, ASTNode *declarator);
SemType sem_resolve(SemAnalyzer *analyzer, SemType type);

SemTag *sem_tag_find(SemAnalyzer *analyzer, const char *name);
SemTag *sem_tag_register(SemAnalyzer *analyzer, ASTNode *type_spec);

SemType sem_check_expr(SemAnalyzer *analyzer, ASTNode *node);
int     sem_is_lvalue(ASTNode *node);
void    sem_check_assign_compat(SemAnalyzer *analyzer, SemType dst, SemType src,
                                ASTNode *src_node, int line, int column, const char *context);

void sem_check_stmt(SemAnalyzer *analyzer, ASTNode *node);
void sem_check_block(SemAnalyzer *analyzer, ASTNode *block, int new_scope);
void sem_check_var_decl(SemAnalyzer *analyzer, ASTNode *node, int is_global);

const char *sem_declarator_name(ASTNode *declarator);
void        sem_node_position(ASTNode *node, int *line, int *column);
int         sem_line(ASTNode *node);
int         sem_col(ASTNode *node);

#endif /* SEM_CHECK_H */
