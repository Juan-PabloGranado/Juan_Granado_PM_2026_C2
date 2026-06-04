#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    int I = 0;
    float SAL, PRO, NOM = 0;
    printf("\nIngrese el salario del profesor:\t");
    scanf("%f", &SAL);
    do
    {
        NOM = NOM + SAL;
        I = I + 1;
        printf("\nIngrese el salario del profesor -0 para terminar-:\t");
        scanf("%f", &SAL);
    }
    while (SAL);
    PRO = NOM / I;
    printf("\nNomina: %.2f \t Promedio de salario: %.2f", NOM, PRO);
}
