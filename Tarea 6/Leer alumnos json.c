/* ============================================================
   leer_alumnos_json.c
   Lee un archivo JSON con estructura:

     {
       "alumnos": [
         {
           "nombre": "...",
           "apellido": "...",
           "promedio": 8.7,
           "materia": "..."
         },
         ...
       ]
     }

   Estrategia (2 pasadas):
     1) Cuenta cuantas veces aparece la clave "nombre" -> n
     2) Reserva memoria dinamica (malloc) para n elementos
     3) Vuelve a recorrer el archivo y llena el arreglo
     4) Imprime el arreglo completo

   Nota: es un parser simple hecho a medida para este formato
   (no interpreta JSON arbitrario, sino el de nuestra estructura Alumno).
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

/* Extrae un valor de tipo cadena: "clave": "valor" -> "valor" */
int extraerCadena(const char *linea, const char *clave, char *destino) {
    char patron[MAX_CAMPO];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    char *pos = strstr(linea, patron);
    if (!pos) return 0;

    pos = strchr(pos + strlen(patron), ':');
    if (!pos) return 0;

    char *comillaInicio = strchr(pos, '"');
    if (!comillaInicio) return 0;
    comillaInicio++;

    char *comillaFin = strchr(comillaInicio, '"');
    if (!comillaFin) return 0;

    size_t len = comillaFin - comillaInicio;
    if (len >= MAX_CAMPO) len = MAX_CAMPO - 1;
    strncpy(destino, comillaInicio, len);
    destino[len] = '\0';
    return 1;
}

/* Extrae un valor numerico: "clave": 8.7 -> 8.7 */
int extraerNumero(const char *linea, const char *clave, float *destino) {
    char patron[MAX_CAMPO];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    char *pos = strstr(linea, patron);
    if (!pos) return 0;

    pos = strchr(pos + strlen(patron), ':');
    if (!pos) return 0;
    pos++;

    *destino = atof(pos);
    return 1;
}

int main(int argc, char *argv[]) {
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.json";
    FILE *archivo = fopen(nombreArchivo, "r");

    if (!archivo) {
        printf("Error: no se pudo abrir el archivo '%s'\n", nombreArchivo);
        return 1;
    }

    /* -------- PRIMERA PASADA: contar elementos (n) -------- */
    char linea[MAX_LINE];
    int n = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strstr(linea, "\"nombre\"")) {
            n++;
        }
    }

    if (n == 0) {
        printf("No se encontraron alumnos en el archivo.\n");
        fclose(archivo);
        return 1;
    }

    printf("Cantidad de alumnos detectados en el JSON: %d\n", n);

    /* -------- RESERVA DE MEMORIA DINAMICA -------- */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (!alumnos) {
        printf("Error: no se pudo reservar memoria.\n");
        fclose(archivo);
        return 1;
    }

    /* -------- SEGUNDA PASADA: cargar los datos -------- */
    rewind(archivo);
    int indice = -1;
    char campo[MAX_CAMPO];
    float valorFloat;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (extraerCadena(linea, "nombre", campo)) {
            indice++;
            if (indice >= n) break;
            strcpy(alumnos[indice].nombre, campo);
        } else if (indice >= 0 && indice < n) {
            if (extraerCadena(linea, "apellido", campo))
                strcpy(alumnos[indice].apellido, campo);
            else if (extraerNumero(linea, "promedio", &valorFloat))
                alumnos[indice].promedio = valorFloat;
            else if (extraerCadena(linea, "materia", campo))
                strcpy(alumnos[indice].materia, campo);
        }
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
