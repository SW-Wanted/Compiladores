#include "math.h"

/**
 * Retorna o valor absoluto (positivo) de um número inteiro.
 */
int abs(int a) 
{
    if (a < 0) 
    {
        return -a;
    }
    return a;
}