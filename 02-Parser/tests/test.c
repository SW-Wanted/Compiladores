int limite = 5;
float fator = 1.5;

int soma(int a, int b) {
    return a + b;
}

int maior(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int main(void) {
    int i = 0;
    int valores[5] = {1, 2, 3, 4, 5};
    int total = 0;
    int resultado = 0;

    for (i = 0; i < limite; i++) {
        total = total + valores[i];
    }

    resultado = soma(total, (int)fator);

    while (resultado > 10) {
        resultado--;
    }

    switch (resultado) {
        case 0:
            resultado = 100;
            break;
        case 1:
            resultado = maior(resultado, total);
            break;
        default:
            resultado = resultado + 1;
            break;
    }

    do {
        resultado = resultado - 1;
    } while (resultado > 0);

    return 0;
}