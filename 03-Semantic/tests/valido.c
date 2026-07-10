/*
 * Programa semanticamente correcto.
 * Exercita: structs, typedef, funcoes, arrays, ponteiros, chamadas,
 * estruturas de controlo e expressoes. Nao deve produzir erros.
 */
#include <stdio.h>

#define TAMANHO 10

struct Ponto {
    int x;
    int y;
};

typedef struct Ponto Ponto;

int soma(int a, int b)
{
    return a + b;
}

float media(float valores[], int n)
{
    float total = 0.0;
    int i;
    for (i = 0; i < n; i = i + 1) {
        total = total + valores[i];
    }
    return total / n;
}

int fatorial(int n)
{
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

int main(void)
{
    int numeros[TAMANHO];
    int i;
    int resultado;
    Ponto p;
    int *ptr;

    p.x = 3;
    p.y = 4;
    ptr = &resultado;

    for (i = 0; i < TAMANHO; i++) {
        numeros[i] = i * 2;
    }

    resultado = soma(numeros[0], numeros[1]);
    *ptr = resultado + p.x;

    if (resultado > 0) {
        printf("positivo: %d\n", resultado);
    } else {
        printf("nao positivo\n");
    }

    while (resultado > 0) {
        resultado = resultado - 1;
    }

    switch (i) {
        case 0:
            resultado = 0;
            break;
        default:
            resultado = fatorial(5);
            break;
    }

    return 0;
}
