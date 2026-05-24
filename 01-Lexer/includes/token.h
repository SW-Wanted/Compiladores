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
/** @brief Palavra reservada double. */
#define TOKEN_DOUBLE         24
/** @brief Palavra reservada long. */
#define TOKEN_LONG           25
/** @brief Palavra reservada short. */
#define TOKEN_SHORT          26
/** @brief Palavra reservada string. */
#define TOKEN_STRING         27
/** @brief Palavra reservada union. */
#define TOKEN_UNION          28
/** @brief Palavra reservada switch. */
#define TOKEN_SWITCH         29
/** @brief Palavra reservada case. */
#define TOKEN_CASE           30
/** @brief Palavra reservada default. */
#define TOKEN_DEFAULT        31
/** @brief Palavra reservada break. */
#define TOKEN_BREAK          32
/** @brief Palavra reservada continue. */
#define TOKEN_CONTINUE       33

/** @brief Operador +. */
#define TOKEN_PLUS           40
/** @brief Operador -. */
#define TOKEN_MINUS          41
/** @brief Operador *. */
#define TOKEN_STAR           42
/** @brief Operador /. */
#define TOKEN_SLASH          43
/** @brief Operador %. */
#define TOKEN_PERCENT        44
/** @brief Operador ++. */
#define TOKEN_PLUS_PLUS      45
/** @brief Operador --. */
#define TOKEN_MINUS_MINUS    46
/** @brief Operador <<. */
#define TOKEN_LSHIFT         47
/** @brief Operador >>. */
#define TOKEN_RSHIFT         48
/** @brief Operador ->. */
#define TOKEN_ARROW          49

/** @brief Operador ==. */
#define TOKEN_EQ             50
/** @brief Operador !=. */
#define TOKEN_NEQ            51
/** @brief Operador <. */
#define TOKEN_LT             52
/** @brief Operador >. */
#define TOKEN_GT             53
/** @brief Operador <=. */
#define TOKEN_LEQ            54
/** @brief Operador >=. */
#define TOKEN_GEQ            55

/** @brief Operador logico &&. */
#define TOKEN_AND            60
/** @brief Operador logico ||. */
#define TOKEN_OR             61
/** @brief Operador logico !. */
#define TOKEN_NOT            62
/** @brief Operador bitwise |. */
#define TOKEN_PIPE           63
/** @brief Operador bitwise ^. */
#define TOKEN_CARET          64

/** @brief Operador de atribuicao =. */
#define TOKEN_ASSIGN         70
/** @brief Operador de atribuicao +=. */
#define TOKEN_PLUS_ASSIGN    71
/** @brief Operador de atribuicao -=. */
#define TOKEN_MINUS_ASSIGN   72
/** @brief Operador de atribuicao *=. */
#define TOKEN_STAR_ASSIGN    73
/** @brief Operador de atribuicao /=. */
#define TOKEN_SLASH_ASSIGN   74
/** @brief Operador de atribuicao %=. */
 #define TOKEN_PERCENT_ASSIGN 75
/** @brief Operador de atribuicao &=. */
#define TOKEN_AMP_ASSIGN     76
/** @brief Operador de atribuicao |=. */
#define TOKEN_PIPE_ASSIGN    77
/** @brief Operador de atribuicao ^=. */
#define TOKEN_CARET_ASSIGN   78
/** @brief Operador de atribuicao <<=. */
#define TOKEN_LSHIFT_ASSIGN  79
/** @brief Operador de atribuicao >>=. */
#define TOKEN_RSHIFT_ASSIGN  80

/** @brief Delimitador (. */
#define TOKEN_LPAREN         90
/** @brief Delimitador ). */
#define TOKEN_RPAREN         91
/** @brief Delimitador {. */
#define TOKEN_LBRACE         92
/** @brief Delimitador }. */
#define TOKEN_RBRACE         93
/** @brief Delimitador [. */
#define TOKEN_LBRACKET       94
/** @brief Delimitador ]. */
#define TOKEN_RBRACKET       95
/** @brief Delimitador ;. */
#define TOKEN_SEMICOLON      96
/** @brief Delimitador ,. */
#define TOKEN_COMMA          97
/** @brief Delimitador .. */
#define TOKEN_DOT            98
/** @brief Delimitador :. */
#define TOKEN_COLON          99
/** @brief Delimitador #. */
#define TOKEN_HASH           100
/** @brief Simbolo &. */
#define TOKEN_AMP            101

/** @brief Fim de arquivo. */
#define TOKEN_EOF            200
/** @brief Token nao reconhecido. */
#define TOKEN_UNKNOWN       201

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
    int  column;
} Token;

/**
 * @brief Converte um tipo numerico de token para nome textual.
 * @param type Tipo numerico do token.
 * @return Nome textual correspondente ao tipo.
 */
const char *token_name(int type);

#endif /* TOKEN_H */
