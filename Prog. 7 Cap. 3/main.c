#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    int I, N;
    float LAN, SLA = 0;
    do
    {
        printf("\nIngrese el numero de lanzaminetos:\t");
        scanf("%d", &N);
    }
    while (N < 1 || N > 11);
    for (I=1; I<=N; I++)
    {
        printf("\nIngrese el lanzamiento %d: ", I);
        scanf("%f", &LAN);
        SLA = SLA + LAN;
    }
    SLA = SLA / N;
    printf("\nEl promedio de lanzamientos es: %.2f", SLA);
}
