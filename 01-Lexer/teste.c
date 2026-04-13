// Programa de teste para o analisador léxico
#include <stdio.h>

int soma(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    float y = 3.14;
    char letra = 'A';
    char nome[] = "ISPTEC";

    if (x >= 10 && y != 0.0) {
        x += 5;
    } else {
        x = 0;
    }

    while (x > 0) {
        x--;
    }

    int resultado = soma(x, 20);
    return 0;
}
