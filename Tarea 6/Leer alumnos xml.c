/* ============================================================
   leer_alumnos_xml.c
   Lee un archivo XML con estructura:

     <alumnos>
       <alumno>
         <nombre>...</nombre>
         <apellido>...</apellido>
         <promedio>...</promedio>
         <materia>...</materia>
       </alumno>
       ...
     </alumnos>

   Estrategia (2 pasadas):
     1) Cuenta cuántas etiquetas <alumno> hay -> n
     2) Reserva memoria dinámica (malloc) para n elementos
     3) Vuelve a recorrer el archivo y llena el arreglo
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

/* Extrae el contenido de una etiqueta simple, ej: <nombre>Ana</nombre> -> "Ana" */
int extraerEtiqueta(const char *linea, const char *tag, char *destino) {
    char apertura[MAX_CAMPO], cierre[MAX_CAMPO];
    snprintf(apertura, sizeof(apertura), "<%s>", tag);
    snprintf(cierre, sizeof(cierre), "</%s>", tag);

    char *inicio = strstr(linea, apertura);
    if (!inicio) return 0;
    inicio += strlen(apertura);

    char *fin = strstr(inicio, cierre);
    if (!fin) return 0;

    size_t len = fin - inicio;
    if (len >= MAX_CAMPO) len = MAX_CAMPO - 1;
    strncpy(destino, inicio, len);
    destino[len] = '\0';
    return 1;
}

int main(int argc, char *argv[]) {
    const char *nombreArchivo = (argc > 1) ? argv[1] : "alumnos.xml";
    FILE *archivo = fopen(nombreArchivo, "r");

    if (!archivo) {
        printf("Error: no se pudo abrir el archivo '%s'\n", nombreArchivo);
        return 1;
    }

    /* -------- PRIMERA PASADA: contar elementos (n) -------- */
    char linea[MAX_LINE];
    int n = 0;

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strstr(linea, "<alumno>")) {
            n++;
        }
    }

    if (n == 0) {
        printf("No se encontraron alumnos en el archivo.\n");
        fclose(archivo);
        return 1;
    }

    printf("Cantidad de alumnos detectados en el XML: %d\n", n);

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

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strstr(linea, "<alumno>")) {
            indice++;
            continue;
        }
        if (indice < 0 || indice >= n) continue;

        if (extraerEtiqueta(linea, "nombre", campo))
            strcpy(alumnos[indice].nombre, campo);
        else if (extraerEtiqueta(linea, "apellido", campo))
            strcpy(alumnos[indice].apellido, campo);
        else if (extraerEtiqueta(linea, "promedio", campo))
            alumnos[indice].promedio = atof(campo);
        else if (extraerEtiqueta(linea, "materia", campo))
            strcpy(alumnos[indice].materia, campo);
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
