#ifndef TOKEN_H
#define TOKEN_H

/* =========================================================
 *  TOKEN TYPES
 *  Each constant represents a category of symbol in C.
 * ========================================================= */

/* --- Literals & Identifiers --- */
#define TOKEN_IDENTIFIER     1
#define TOKEN_INT_LITERAL    2
#define TOKEN_FLOAT_LITERAL  3
#define TOKEN_CHAR_LITERAL   4
#define TOKEN_STRING_LITERAL 5

/* --- Keywords --- */
#define TOKEN_INT            10
#define TOKEN_FLOAT          11
#define TOKEN_CHAR           12
#define TOKEN_VOID           13
#define TOKEN_IF             14
#define TOKEN_ELSE           15
#define TOKEN_WHILE          16
#define TOKEN_FOR            17
#define TOKEN_DO             18
#define TOKEN_RETURN         19
#define TOKEN_STRUCT         20
#define TOKEN_TYPEDEF        21
#define TOKEN_INCLUDE        22
#define TOKEN_DEFINE         23

/* --- Arithmetic Operators --- */
#define TOKEN_PLUS           30
#define TOKEN_MINUS          31
#define TOKEN_STAR           32
#define TOKEN_SLASH          33
#define TOKEN_PERCENT        34

/* --- Relational Operators --- */
#define TOKEN_EQ             40   /* == */
#define TOKEN_NEQ            41   /* != */
#define TOKEN_LT             42   /* <  */
#define TOKEN_GT             43   /* >  */
#define TOKEN_LEQ            44   /* <= */
#define TOKEN_GEQ            45   /* >= */

/* --- Logical Operators --- */
#define TOKEN_AND            50   /* && */
#define TOKEN_OR             51   /* || */
#define TOKEN_NOT            52   /* !  */

/* --- Assignment --- */
#define TOKEN_ASSIGN         60   /* =  */
#define TOKEN_PLUS_ASSIGN    61   /* += */
#define TOKEN_MINUS_ASSIGN   62   /* -= */

/* --- Delimiters & Punctuation --- */
#define TOKEN_LPAREN         70   /* (  */
#define TOKEN_RPAREN         71   /* )  */
#define TOKEN_LBRACE         72   /* {  */
#define TOKEN_RBRACE         73   /* }  */
#define TOKEN_LBRACKET       74   /* [  */
#define TOKEN_RBRACKET       75   /* ]  */
#define TOKEN_SEMICOLON      76   /* ;  */
#define TOKEN_COMMA          77   /* ,  */
#define TOKEN_DOT            78   /* .  */
#define TOKEN_COLON          79   /* :  */
#define TOKEN_HASH           80   /* #  */
#define TOKEN_AMP            81   /* &  */

/* --- Special --- */
#define TOKEN_EOF            99
#define TOKEN_UNKNOWN       100

/* =========================================================
 *  TOKEN STRUCT
 *  One token = type + lexeme (the actual text) + line number.
 * ========================================================= */

#define MAX_LEXEME_LEN 256

typedef struct {
    int  type;
    char lexeme[MAX_LEXEME_LEN];
    int  line;
} Token;

/* Returns a human-readable name for a token type. */
const char *token_name(int type);

#endif /* TOKEN_H */
