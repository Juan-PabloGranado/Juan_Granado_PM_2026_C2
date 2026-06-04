#include <stdio.h>
#include <stdlib.h>
#define pow

void main(void)
{
    int NUM;
    printf("\nIngrese el numero: ");
    scanf("%d", &NUM);
    if (NUM == 0)
        printf("\nNulo");
    else
        if (pow (-


                 1, NUM) > 0)
            printf("\nPar");
        else
            printf("\nImpar");
}
