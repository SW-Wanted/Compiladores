#ifndef TOKEN_H
#define TOKEN_H

/** @brief Identificador generico. */
#define TOKEN_IDENTIFIER     1
/** @brief Literal inteiro. */
#define TOKEN_INT_LITERAL    2
/** @brief Literal de ponto flutuante. */
#define TOKEN_FLOAT_LITERAL  3
/** @brief Literal de caractere. */
#define TOKEN_CHAR_LITERAL   4
/** @brief Literal de string. */
#define TOKEN_STRING_LITERAL 5

/** @brief Palavra reservada int. */
#define TOKEN_INT            10
/** @brief Palavra reservada float. */
#define TOKEN_FLOAT          11
/** @brief Palavra reservada char. */
#define TOKEN_CHAR           12
/** @brief Palavra reservada void. */
#define TOKEN_VOID           13
/** @brief Palavra reservada if. */
#define TOKEN_IF             14
/** @brief Palavra reservada else. */
#define TOKEN_ELSE           15
/** @brief Palavra reservada while. */
#define TOKEN_WHILE          16
/** @brief Palavra reservada for. */
#define TOKEN_FOR            17
/** @brief Palavra reservada do. */
#define TOKEN_DO             18
/** @brief Palavra reservada return. */
#define TOKEN_RETURN         19
/** @brief Palavra reservada struct. */
#define TOKEN_STRUCT         20
/** @brief Palavra reservada typedef. */
#define TOKEN_TYPEDEF        21
/** @brief Diretiva include. */
#define TOKEN_INCLUDE        22
/** @brief Diretiva define. */
#define TOKEN_DEFINE         23

/** @brief Operador +. */
#define TOKEN_PLUS           30
/** @brief Operador -. */
#define TOKEN_MINUS          31
/** @brief Operador *. */
#define TOKEN_STAR           32
/** @brief Operador /. */
#define TOKEN_SLASH          33
/** @brief Operador %. */
#define TOKEN_PERCENT        34

/** @brief Operador ==. */
#define TOKEN_EQ             40
/** @brief Operador !=. */
#define TOKEN_NEQ            41
/** @brief Operador <. */
#define TOKEN_LT             42
/** @brief Operador >. */
#define TOKEN_GT             43
/** @brief Operador <=. */
#define TOKEN_LEQ            44
/** @brief Operador >=. */
#define TOKEN_GEQ            45

/** @brief Operador logico &&. */
#define TOKEN_AND            50
/** @brief Operador logico ||. */
#define TOKEN_OR             51
/** @brief Operador logico !. */
#define TOKEN_NOT            52

/** @brief Operador de atribuicao =. */
#define TOKEN_ASSIGN         60
/** @brief Operador de atribuicao +=. */
#define TOKEN_PLUS_ASSIGN    61
/** @brief Operador de atribuicao -=. */
#define TOKEN_MINUS_ASSIGN   62

/** @brief Delimitador (. */
#define TOKEN_LPAREN         70
/** @brief Delimitador ). */
#define TOKEN_RPAREN         71
/** @brief Delimitador {. */
#define TOKEN_LBRACE         72
/** @brief Delimitador }. */
#define TOKEN_RBRACE         73
/** @brief Delimitador [. */
#define TOKEN_LBRACKET       74
/** @brief Delimitador ]. */
#define TOKEN_RBRACKET       75
/** @brief Delimitador ;. */
#define TOKEN_SEMICOLON      76
/** @brief Delimitador ,. */
#define TOKEN_COMMA          77
/** @brief Delimitador .. */
#define TOKEN_DOT            78
/** @brief Delimitador :. */
#define TOKEN_COLON          79
/** @brief Delimitador #. */
#define TOKEN_HASH           80
/** @brief Simbolo &. */
#define TOKEN_AMP            81

/** @brief Fim de arquivo. */
#define TOKEN_EOF            99
/** @brief Token nao reconhecido. */
#define TOKEN_UNKNOWN       100

/**
 * @brief Tamanho maximo do lexema armazenado em um token.
 */

#define MAX_LEXEME_LEN 256

/**
 * @brief Estrutura de token reconhecido pelo lexer.
 */
typedef struct {
    int  type;
    char lexeme[MAX_LEXEME_LEN];
    int  line;
} Token;

/**
 * @brief Converte um tipo numerico de token para nome textual.
 * @param type Tipo numerico do token.
 * @return Nome textual correspondente ao tipo.
 */
const char *token_name(int type);

#endif /* TOKEN_H */
