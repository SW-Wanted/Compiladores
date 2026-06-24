#ifndef PARSER_STATEMENTS_H
#define PARSER_STATEMENTS_H

#include "parser.h"

ASTNode *parse_bloco(Parser *parser);
ASTNode *parse_bloco_interno(Parser *parser);
ASTNode *parse_item_bloco(Parser *parser);
ASTNode *parse_declaracao_variavel_local(Parser *parser);
ASTNode *parse_instrucao(Parser *parser);
ASTNode *parse_instrucao_expressao(Parser *parser);
ASTNode *parse_instrucao_if(Parser *parser);
ASTNode *parse_ramo_else_opcional(Parser *parser);
ASTNode *parse_instrucao_while(Parser *parser);
ASTNode *parse_instrucao_for(Parser *parser);
ASTNode *parse_instrucao_do(Parser *parser);
ASTNode *parse_instrucao_switch(Parser *parser);
ASTNode *parse_case_item(Parser *parser);
ASTNode *parse_instrucao_break(Parser *parser);
ASTNode *parse_instrucao_continue(Parser *parser);
ASTNode *parse_instrucao_return(Parser *parser);
ASTNode *parse_expressao_opcional(Parser *parser);

#endif
