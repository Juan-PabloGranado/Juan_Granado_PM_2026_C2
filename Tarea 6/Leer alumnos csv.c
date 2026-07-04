/* ============================================================
   leer_alumnos_csv.c
   Lee un archivo CSV con estructura:

     Nombre,Apellido,Promedio,Materia
     Ana,Martínez,8.7,Matematicas
     ...

   Estrategia (2 pasadas):
     1) Cuenta cuantas lineas de datos hay (total de lineas - 1 de encabezado) -> n
     2) Reserva memoria dinamica (malloc) para n elementos
     3) Vuelve a recorrer el archivo, salta el encabezado y llena el arreglo
     4) Imprime el arreglo completo
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_CAMPO 100

typedef struct {
    char nombre[MAX_CAMPO];
    char apellido[MAX_CAMPO];
    float promedio;
    char materia[MAX_CAMPO];
} Alumno;

int main(int argc, char *argv[]) {
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.csv";
    FILE *archivo = fopen(nombreArchivo, "r");

    if (!archivo) {
        printf("Error: no se pudo abrir el archivo '%s'\n", nombreArchivo);
        return 1;
    }

    /* -------- PRIMERA PASADA: contar elementos (n) -------- */
    char linea[MAX_LINE];
    int totalLineas = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        totalLineas++;
    }

    int n = totalLineas - 1; /* se resta la linea del encabezado */

    if (n <= 0) {
        printf("No se encontraron alumnos en el archivo.\n");
        fclose(archivo);
        return 1;
    }

    printf("Cantidad de alumnos detectados en el CSV: %d\n", n);

    /* -------- RESERVA DE MEMORIA DINAMICA -------- */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (!alumnos) {
        printf("Error: no se pudo reservar memoria.\n");
        fclose(archivo);
        return 1;
    }

    /* -------- SEGUNDA PASADA: cargar los datos -------- */
    rewind(archivo);
    fgets(linea, sizeof(linea), archivo); /* descartar encabezado */

    int indice = 0;
    while (fgets(linea, sizeof(linea), archivo) && indice < n) {
        /* quitar el salto de linea final, si existe */
        linea[strcspn(linea, "\r\n")] = '\0';

        char *token = strtok(linea, ",");
        if (token) strncpy(alumnos[indice].nombre, token, MAX_CAMPO - 1);

        token = strtok(NULL, ",");
        if (token) strncpy(alumnos[indice].apellido, token, MAX_CAMPO - 1);

        token = strtok(NULL, ",");
        if (token) alumnos[indice].promedio = atof(token);

        token = strtok(NULL, ",");
        if (token) strncpy(alumnos[indice].materia, token, MAX_CAMPO - 1);

        indice++;
    }

    fclose(archivo);

    /* -------- IMPRESION -------- */
    printf("\n%-15s %-15s %-10s %-20s\n", "Nombre", "Apellido", "Promedio", "Materia");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-15s %-15s %-10.1f %-20s\n",
               alumnos[i].nombre, alumnos[i].apellido,
               alumnos[i].promedio, alumnos[i].materia);
    }

    free(alumnos); /* liberar memoria dinamica */
    return 0;
}
