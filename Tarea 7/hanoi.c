/*
 * Torres de Hanoi
 * -----------------
 * Resuelve el problema de las Torres de Hanoi para N discos de forma
 * recursiva y va guardando cada movimiento, numerado, en un archivo
 * de texto llamado "movimientos.txt".
 *
 * Compilar:  gcc hanoi.c -o hanoi
 * Ejecutar:  ./hanoi
 */

#include <stdio.h>
#include <stdlib.h>

/* Contador global de movimientos (empieza en 1) */
int contadorMovimientos = 1;

/* Puntero al archivo donde se registran los movimientos */
FILE *archivo;

/*
 * moverDiscos:
 *   n       -> cantidad de discos a mover
 *   origen  -> poste de donde salen los discos
 *   auxiliar-> poste que se usa de apoyo
 *   destino -> poste a donde deben llegar los discos
 */
void moverDiscos(int n, char origen, char auxiliar, char destino) {
    if (n == 0) {
        return; /* Caso base: no hay nada que mover */
    }

    /* 1. Mover los n-1 discos de arriba de origen -> auxiliar */
    moverDiscos(n - 1, origen, destino, auxiliar);

    /* 2. Mover el disco más grande de origen -> destino,
          y registrar el movimiento en el archivo */
    printf("Movimiento %d: Mover disco %d de %c a %c\n",
           contadorMovimientos, n, origen, destino);

    fprintf(archivo, "%d. Mover disco %d de %c a %c\n",
            contadorMovimientos, n, origen, destino);

    contadorMovimientos++;

    /* 3. Mover los n-1 discos de auxiliar -> destino */
    moverDiscos(n - 1, auxiliar, origen, destino);
}

int main(void) {
    int discos;

    printf("=== Torres de Hanoi ===\n");
    printf("Ingrese el numero de discos: ");
    if (scanf("%d", &discos) != 1 || discos <= 0) {
        printf("Numero de discos invalido.\n");
        return 1;
    }

    /* Abrir (crear) el archivo de texto donde se registraran los movimientos */
    archivo = fopen("movimientos.txt", "w");
    if (archivo == NULL) {
        printf("Error: no se pudo crear el archivo movimientos.txt\n");
        return 1;
    }

    fprintf(archivo, "Registro de movimientos - Torres de Hanoi (%d discos)\n",
            discos);
    fprintf(archivo, "------------------------------------------------------\n");

    /* Postes: A = origen, B = auxiliar, C = destino */
    moverDiscos(discos, 'A', 'B', 'C');

    fprintf(archivo, "------------------------------------------------------\n");
    fprintf(archivo, "Total de movimientos: %d\n", contadorMovimientos - 1);

    fclose(archivo);

    printf("\nTotal de movimientos: %d\n", contadorMovimientos - 1);
    printf("Movimientos guardados en 'movimientos.txt'\n");

    return 0;
}
