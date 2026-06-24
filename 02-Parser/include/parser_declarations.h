#ifndef PARSER_DECLARATIONS_H
#define PARSER_DECLARATIONS_H

#include "parser.h"

ASTNode *parse_programa(Parser *parser);
ASTNode *parse_lista_declaracoes_globais(Parser *parser);
ASTNode *parse_declaracao_global(Parser *parser);
ASTNode *parse_directiva(Parser *parser);
ASTNode *parse_diretiva_include(Parser *parser);
ASTNode *parse_diretiva_define(Parser *parser);
ASTNode *parse_nome_ficheiro(Parser *parser);
ASTNode *parse_type_specifier(Parser *parser);
ASTNode *parse_nome_ou_corpo_struct(Parser *parser);
ASTNode *parse_corpo_campos(Parser *parser);
ASTNode *parse_campo(Parser *parser);
ASTNode *parse_lista_declaradores_campo(Parser *parser);
ASTNode *parse_declaracao_typedef(Parser *parser);
ASTNode *parse_declaracao_geral(Parser *parser);
ASTNode *parse_declarator(Parser *parser);
ASTNode *parse_sufixo_array_opcional(Parser *parser);
ASTNode *parse_inicializacao_opcional(Parser *parser);
ASTNode *parse_mais_declaradores(Parser *parser);
ASTNode *parse_parametros_opcionais(Parser *parser);
ASTNode *parse_lista_parametros(Parser *parser);
ASTNode *parse_parametro(Parser *parser);
ASTNode *parse_resto_parametro(Parser *parser);

#endif
