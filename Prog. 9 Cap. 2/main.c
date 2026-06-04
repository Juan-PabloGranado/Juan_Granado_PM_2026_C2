#include <stdio.h>
#include <stdlib.h>
#define pow

void main(void)
{
    float RES;
    int R, T, Q;
    printf("\nIngrese los valores de R, T, Q: ");
    scanf("%d %d %d", &R, &T, &Q);
    RES = pow(R, 4) - pow(T, 3) + 4 * pow(Q, 2);
    if (RES < 820)
        printf("\nR = %d\t Q = %d", R, T, Q);
}
