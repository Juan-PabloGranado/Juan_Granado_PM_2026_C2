#include <stdio.h>
#include <stdlib.h>

int main()
{
    int dato = -1;
    printf("\nIngrese un numero del 1 al 10: ");
    scanf("%i",&dato);
    if(dato >= 1 && dato <= 10)
    {
        if(dato == 5)
        {
            printf("\nAdivinaste el numero 5");

        }else if(dato == 7)
        {
            printf("\nAdivinaste el numero 7");
        }else
        {
            printf("\nPerdiste!");
        }
    }else
    {
        printf("\nNumero no valido!");
    }
}
