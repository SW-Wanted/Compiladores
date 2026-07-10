#include <string.h>
#include <stdio.h>

#include "sem_check.h"

/* ------------------------------------------------------------------ */
/*  Utilitarios de navegacao na AST                                    */
/* ------------------------------------------------------------------ */

const char *sem_declarator_name(ASTNode *node)
{
    if (!node)
        return NULL;
    if (node->kind == AST_IDENTIFIER)
        return node->text;
    for (int i = 0; i < node->child_count; i++) {
        const char *name = sem_declarator_name(node->children[i]);
        if (name)
            return name;
    }
    return NULL;
}

void sem_node_position(ASTNode *node, int *line, int *column)
{
    if (!node) {
        *line = -1;
        *column = -1;
        return;
    }
    if (node->line >= 0) {
        *line = node->line;
        *column = node->column;
        return;
    }
    /* No sem posicao propria: procura a primeira posicao valida na subarvore. */
    for (int i = 0; i < node->child_count; i++) {
        sem_node_position(node->children[i], line, column);
        if (*line >= 0)
            return;
    }
    *line = -1;
    *column = -1;
}

int sem_line(ASTNode *node)
{
    int line = -1, column = -1;
    sem_node_position(node, &line, &column);
    return line;
}

int sem_col(ASTNode *node)
{
    int line = -1, column = -1;
    sem_node_position(node, &line, &column);
    return column;
}

/* ------------------------------------------------------------------ */
/*  Qualificadores e tipos base                                        */
/* ------------------------------------------------------------------ */

static int is_qualifier(const char *text)
{
    if (!text)
        return 0;
    return strcmp(text, "const") == 0 || strcmp(text, "static") == 0 ||
           strcmp(text, "unsigned") == 0 || strcmp(text, "signed") == 0 ||
           strcmp(text, "volatile") == 0 || strcmp(text, "extern") == 0 ||
           strcmp(text, "register") == 0 || strcmp(text, "auto") == 0;
}

static int base_from_text(const char *text)
{
    if (!text) return TOKEN_INT;
    if (strcmp(text, "int") == 0)    return TOKEN_INT;
    if (strcmp(text, "float") == 0)  return TOKEN_FLOAT;
    if (strcmp(text, "char") == 0)   return TOKEN_CHAR;
    if (strcmp(text, "void") == 0)   return TOKEN_VOID;
    if (strcmp(text, "double") == 0) return TOKEN_DOUBLE;
    if (strcmp(text, "long") == 0)   return TOKEN_LONG;
    if (strcmp(text, "short") == 0)  return TOKEN_SHORT;
    if (strcmp(text, "struct") == 0) return TOKEN_STRUCT;
    if (strcmp(text, "union") == 0)  return TOKEN_UNION;
    return TOKEN_IDENTIFIER; /* typedef-name ou tipo externo */
}

/* ------------------------------------------------------------------ */
/*  Etiquetas struct / union                                           */
/* ------------------------------------------------------------------ */

SemTag *sem_tag_find(SemAnalyzer *analyzer, const char *name)
{
    if (!name || !name[0])
        return NULL;
    for (int i = 0; i < analyzer->tag_count; i++) {
        if (strcmp(analyzer->tags[i].name, name) == 0)
            return &analyzer->tags[i];
    }
    return NULL;
}

static SemTag *tag_create(SemAnalyzer *analyzer, const char *name, int is_union)
{
    if (analyzer->tag_count >= SEM_MAX_TAGS)
        return NULL;
    SemTag *tag = &analyzer->tags[analyzer->tag_count++];
    memset(tag, 0, sizeof(*tag));
    strncpy(tag->name, name, SEM_MAX_NAME - 1);
    tag->is_union = is_union;
    tag->defined = 0;
    tag->field_count = 0;
    return tag;
}

/* Regista os campos declarados no corpo (AST_BLOCK) de uma struct/union. */
static void register_fields(SemAnalyzer *analyzer, SemTag *tag, ASTNode *body)
{
    for (int i = 0; i < body->child_count; i++) {
        ASTNode *field = body->children[i];
        if (!field || field->kind != AST_VAR_DECL || field->child_count < 2)
            continue;

        SemType base = sem_type_from_spec(analyzer, field->children[0]);
        ASTNode *decls = field->children[1];

        if (decls->kind == AST_DECLARATOR_LIST) {
            for (int j = 0; j < decls->child_count; j++) {
                ASTNode *declarator = decls->children[j];
                if (declarator->kind != AST_DECLARATOR)
                    continue;
                const char *name = sem_declarator_name(declarator);
                if (!name || tag->field_count >= SEM_MAX_FIELDS)
                    continue;
                strncpy(tag->field_names[tag->field_count], name, SEM_MAX_NAME - 1);
                tag->field_types[tag->field_count] = sem_type_from_declarator(base, declarator);
                tag->field_count++;
            }
        } else if (decls->kind == AST_DECLARATOR) {
            const char *name = sem_declarator_name(decls);
            if (name && tag->field_count < SEM_MAX_FIELDS) {
                strncpy(tag->field_names[tag->field_count], name, SEM_MAX_NAME - 1);
                tag->field_types[tag->field_count] = sem_type_from_declarator(base, decls);
                tag->field_count++;
            }
        }
    }
}

SemTag *sem_tag_register(SemAnalyzer *analyzer, ASTNode *type_spec)
{
    int is_union = (type_spec->text && strcmp(type_spec->text, "union") == 0);
    if (type_spec->child_count == 0)
        return NULL;

    ASTNode *body = NULL;
    char tag_name[SEM_MAX_NAME];
    tag_name[0] = '\0';

    ASTNode *info = type_spec->children[0];
    if (info->kind == AST_IDENTIFIER) {
        /* nome (+ corpo opcional): info->children = [nome, corpo?] */
        if (info->child_count > 0 && info->children[0]->text)
            strncpy(tag_name, info->children[0]->text, SEM_MAX_NAME - 1);
        for (int i = 1; i < info->child_count; i++) {
            if (info->children[i]->kind == AST_BLOCK) {
                body = info->children[i];
                break;
            }
        }
    } else if (info->kind == AST_BLOCK) {
        /* struct/union anonima */
        snprintf(tag_name, sizeof(tag_name), "@anon%d", ++analyzer->anon_counter);
        body = info;
    }

    if (!tag_name[0])
        return NULL;

    SemTag *tag = sem_tag_find(analyzer, tag_name);
    if (!tag)
        tag = tag_create(analyzer, tag_name, is_union);
    if (!tag)
        return NULL;

    if (body && !tag->defined) {
        tag->defined = 1;      /* marcar antes de registar campos (structs auto-referentes) */
        register_fields(analyzer, tag, body);
    }
    return tag;
}

/* ------------------------------------------------------------------ */
/*  Construcao de tipos a partir da AST                                */
/* ------------------------------------------------------------------ */

SemType sem_type_from_spec(SemAnalyzer *analyzer, ASTNode *type_spec)
{
    if (!type_spec)
        return sem_type_int();

    /* Qualificador de armazenamento/const: descer ao tipo interno. */
    if (is_qualifier(type_spec->text)) {
        if (type_spec->child_count > 0)
            return sem_type_from_spec(analyzer, type_spec->children[0]);
        return sem_type_int();
    }

    int base = base_from_text(type_spec->text);

    if (base == TOKEN_STRUCT || base == TOKEN_UNION) {
        SemTag *tag = sem_tag_register(analyzer, type_spec);
        SemType t = sem_type_make(base, tag ? tag->name : NULL);
        return t;
    }

    if (base == TOKEN_IDENTIFIER)
        return sem_type_make(TOKEN_IDENTIFIER, type_spec->text);

    return sem_type_make(base, NULL);
}

SemType sem_type_from_declarator(SemType base, ASTNode *declarator)
{
    SemType t = base;
    if (!declarator)
        return t;

    for (int i = 0; i < declarator->child_count; i++) {
        ASTNode *child = declarator->children[i];
        if (child->kind == AST_POINTER) {
            t.pointer_level++;
        } else if (child->kind == AST_ARRAY) {
            int dims = child->child_count > 0 ? child->child_count : 1;
            t.array_level += dims;
        }
    }
    return t;
}

SemType sem_resolve(SemAnalyzer *analyzer, SemType type)
{
    int guard = 0;
    while (type.valid && type.base == TOKEN_IDENTIFIER && type.name[0] && guard++ < 16) {
        SemSymbol *sym = sem_lookup(&analyzer->symtab, type.name);
        if (!sym || sym->kind != SYM_TYPEDEF)
            break;
        SemType underlying = sym->type;
        underlying.pointer_level += type.pointer_level;
        underlying.array_level += type.array_level;
        type = underlying;
    }
    return type;
}
