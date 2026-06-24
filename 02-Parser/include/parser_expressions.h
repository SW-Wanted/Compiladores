#ifndef PARSER_EXPRESSIONS_H
#define PARSER_EXPRESSIONS_H

#include "parser.h"

ASTNode *parse_expressao(Parser *parser);
ASTNode *parse_atribuicao(Parser *parser);
ASTNode *parse_condicional(Parser *parser);
ASTNode *parse_logico_ou(Parser *parser);
ASTNode *parse_logico_e(Parser *parser);
ASTNode *parse_bitwise_ou(Parser *parser);
ASTNode *parse_bitwise_xor(Parser *parser);
ASTNode *parse_bitwise_e(Parser *parser);
ASTNode *parse_igualdade(Parser *parser);
ASTNode *parse_relacional(Parser *parser);
ASTNode *parse_shift(Parser *parser);
ASTNode *parse_aditivo(Parser *parser);
ASTNode *parse_multiplicativo(Parser *parser);
ASTNode *parse_unario(Parser *parser);
ASTNode *parse_pos_fixo(Parser *parser);
ASTNode *parse_primario(Parser *parser);
ASTNode *parse_argumentos_opcionais(Parser *parser);

#endif
