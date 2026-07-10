#ifndef SEM_CHECK_H
#define SEM_CHECK_H

#include "semantic.h"

/**
 * @file sem_check.h
 * @brief Prototipos internos partilhados entre os modulos de verificacao
 *        (declaracoes, instrucoes e expressoes) do analisador semantico.
 */

/* ---- Tipos: construcao a partir da AST e resolucao ---- */

/** @brief Constroi o @ref SemType base a partir de um no `AST_TYPE_SPECIFIER`. */
SemType sem_type_from_spec(SemAnalyzer *analyzer, ASTNode *type_spec);
/** @brief Aplica os ponteiros/arrays de um `AST_DECLARATOR` a um tipo base. */
SemType sem_type_from_declarator(SemType base, ASTNode *declarator);
/** @brief Resolve typedefs, devolvendo o tipo subjacente. */
SemType sem_resolve(SemAnalyzer *analyzer, SemType type);

/* ---- Etiquetas struct/union ---- */

/** @brief Procura uma etiqueta pelo nome (NULL se nao existir). */
SemTag *sem_tag_find(SemAnalyzer *analyzer, const char *name);
/** @brief Regista (ou obtem) uma etiqueta e, se houver corpo, os seus campos. */
SemTag *sem_tag_register(SemAnalyzer *analyzer, ASTNode *type_spec);

/* ---- Verificacao de expressoes ---- */

/**
 * @brief Verifica uma expressao e devolve o seu tipo, emitindo diagnosticos.
 */
SemType sem_check_expr(SemAnalyzer *analyzer, ASTNode *node);
/** @brief Verdadeiro se a expressao e um lvalue (pode aparecer a esquerda de `=`). */
int     sem_is_lvalue(ASTNode *node);
/**
 * @brief Verifica a compatibilidade de uma atribuicao/inicializacao/argumento
 *        entre o tipo de destino @p dst e o tipo de origem @p src, emitindo o
 *        diagnostico adequado. @p context descreve o local (ex.: "return",
 *        "argumento 2 de 'f'", "inicializacao de 'x'").
 */
void sem_check_assign_compat(SemAnalyzer *analyzer, SemType dst, SemType src,
                             ASTNode *src_node, int line, int column, const char *context);

/* ---- Verificacao de instrucoes e declaracoes ---- */

/** @brief Verifica uma instrucao (statement). */
void sem_check_stmt(SemAnalyzer *analyzer, ASTNode *node);
/**
 * @brief Verifica um bloco.
 * @param new_scope Se 1, abre/fecha um escopo proprio; se 0, usa o escopo actual
 *        (usado para o corpo de funcao, que partilha o escopo dos parametros).
 */
void sem_check_block(SemAnalyzer *analyzer, ASTNode *block, int new_scope);
/** @brief Verifica uma declaracao de variavel local (`AST_VAR_DECL`). */
void sem_check_var_decl(SemAnalyzer *analyzer, ASTNode *node, int is_global);

/** @brief Localiza o no do declarador com o nome, devolvendo linha/coluna. */
const char *sem_declarator_name(ASTNode *declarator);
/** @brief Resolve a posicao de um no (ou a primeira valida na subarvore). */
void        sem_node_position(ASTNode *node, int *line, int *column);
/** @brief Linha resolvida de um no (subarvore incluida). */
int         sem_line(ASTNode *node);
/** @brief Coluna resolvida de um no (subarvore incluida). */
int         sem_col(ASTNode *node);

#endif /* SEM_CHECK_H */
