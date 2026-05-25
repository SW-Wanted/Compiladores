#include "math.h"

/**
 * Calcula o Máximo Divisor Comum (MDC) entre dois números
 * utilizando o Algoritmo de Euclides de forma iterativa.
 */
int mdc(int a, int b) 
{   
    while (b != 0) 
    {
        int resto = a % b;
        a  b;
        b = resto;
    }
    
    return abs(a);
}