/*
 * Programa com varios erros semanticos propositais.
 * Cada linha assinalada deve gerar o diagnostico correspondente.
 */
struct Caixa {
    int valor;
};

int soma(int a, int b)
{
    return a + b;
}

void proc(void)
{
    return 7;                 /* ERRO: funcao void nao pode retornar valor */
}

int global;
int global;                   /* ERRO: redeclaracao no escopo global */

int main(void)
{
    int x;
    int x;                    /* ERRO: redeclaracao no mesmo escopo */
    int y;
    struct Caixa c;

    y = naoexiste;            /* ERRO: variavel nao declarada */
    x = "texto";              /* ERRO: incompatibilidade de tipos (char* -> int) */

    soma(1);                  /* ERRO: numero de argumentos (espera 2, recebe 1) */
    soma(1, 2, 3);            /* ERRO: numero de argumentos (espera 2, recebe 3) */
    soma(1, c);               /* ERRO: argumento 2 incompativel (struct -> int) */

    if (c) {                  /* ERRO: condicao deve ser escalar */
        x = 1;
    }

    break;                    /* ERRO: 'break' fora de ciclo/switch */
    continue;                 /* ERRO: 'continue' fora de ciclo */

    x = 3.5;                  /* AVISO: conversao double -> int (perda) */

    return 0;
}
