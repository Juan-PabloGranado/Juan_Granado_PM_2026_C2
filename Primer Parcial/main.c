#include <stdio.h>
#include <stdlib.h>

int main()
{
    int opcion;
    int cantidad_jugadas = 0;
    int numero_jugado;
    float dinero_jugado;
    float premio;

    do
    {
        printf("\n--- Menu de Loteria ---\n");
        printf("\n1. Jugar");
        printf("\n2. Elegir cantidad de jugadas");
        printf("\n3. Salir");
        printf("\nSeleccione una opcion: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
        case 1:
            if (cantidad_jugadas <= 0)
            {
                printf("\nError: Debe elegir la cantidad de jugadas primero (opcion 2).");
            }else
            {
                printf("Ingrese su numero del 1 al 40: ");
                scanf("%d", &numero_jugado);

                if (numero_jugado >= 1 && numero_jugado <= 40)
                {
                    printf("\nIngrese la cantidad de dinero a jugar: ");
                    scanf("%f", &dinero_jugado);
                    premio = dinero_jugado * 1000;
                    printf("\nJugada registrada. El premio potencial es: %.2f", premio);
                    cantidad_jugadas--;
                }else
                {
                    printf("\nNumero invalido. Debe ser entre 1 y 40");
                }
            }break;

        case 2:
            printf("\nIngrese la cantidad de jugadas permitidas: ");
            scanf("%d", &cantidad_jugadas);
            printf("\nSe han configurado %d jugadas.", cantidad_jugadas);
            break;

        case 3:
            printf("\nSaliendo del programa...");
            break;

        default:
            printf("\nOpcion no valida");

        }


    } while (opcion != 3);

    return 0;
}
