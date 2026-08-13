/*
 * Programa unico con un menu principal que combina:
 *
 *   1) Convertir imagenes PNG/JPG/BMP a blanco y negro
 *   2) Identificar archivos
 *
 *
 * Compilar:  gcc programa_unificado.c -o programa_unificado -lm
 * Ejecutar:  ./programa_unificado
 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_ARCHIVOS 200
#define MAX_NOMBRE   256
#define BYTES_FIRMA  12

/* ============================================================
 *  UTILIDADES COMUNES (nombres, extensiones, listado de carpeta)
 * ============================================================ */

void obtenerExtension(const char *nombre, char *ext, size_t tam) {
    const char *punto = strrchr(nombre, '.');
    if (punto == NULL) {
        ext[0] = '\0';
        return;
    }
    punto++;
    size_t i = 0;
    while (punto[i] && i < tam - 1) {
        ext[i] = (char) tolower((unsigned char) punto[i]);
        i++;
    }
    ext[i] = '\0';
}

/* Lista TODOS los archivos de la carpeta actual (usada por el
 * identificador de archivos). */
int listarArchivos(char lista[][MAX_NOMBRE], int maxArchivos) {
    DIR *dir = opendir(".");
    struct dirent *entrada;
    int total = 0;

    if (dir == NULL) {
        printf("No se pudo abrir el directorio actual.\n");
        return 0;
    }

    while ((entrada = readdir(dir)) != NULL && total < maxArchivos) {
        if (entrada->d_name[0] == '.') continue; /* ocultar . .. y ocultos */
        strncpy(lista[total], entrada->d_name, MAX_NOMBRE - 1);
        lista[total][MAX_NOMBRE - 1] = '\0';
        total++;
    }

    closedir(dir);
    return total;
}

/* Devuelve 1 si el archivo tiene una extension de imagen soportada
 * (usada por el conversor a blanco y negro). */
int esImagenSoportada(const char *nombre) {
    char ext[16];
    obtenerExtension(nombre, ext, sizeof(ext));
    return (strcmp(ext, "png") == 0 ||
            strcmp(ext, "jpg") == 0 ||
            strcmp(ext, "jpeg") == 0 ||
            strcmp(ext, "bmp") == 0);
}

/* Lista solo los archivos de imagen soportados. */
int listarImagenes(char lista[][MAX_NOMBRE], int maxArchivos) {
    DIR *dir = opendir(".");
    struct dirent *entrada;
    int total = 0;

    if (dir == NULL) {
        printf("No se pudo abrir el directorio actual.\n");
        return 0;
    }

    while ((entrada = readdir(dir)) != NULL && total < maxArchivos) {
        if (esImagenSoportada(entrada->d_name)) {
            strncpy(lista[total], entrada->d_name, MAX_NOMBRE - 1);
            lista[total][MAX_NOMBRE - 1] = '\0';
            total++;
        }
    }

    closedir(dir);
    return total;
}

/* ============================================================
 *  MODULO 1: CONVERTIR IMAGENES A BLANCO Y NEGRO
 * ============================================================ */

void construirNombreSalida(const char *original, char *salida, size_t tam) {
    char base[MAX_NOMBRE];
    strncpy(base, original, MAX_NOMBRE - 1);
    base[MAX_NOMBRE - 1] = '\0';

    char *punto = strrchr(base, '.');
    if (punto != NULL) {
        char extension[16];
        strncpy(extension, punto + 1, sizeof(extension) - 1);
        extension[sizeof(extension) - 1] = '\0';

        *punto = '\0';
        snprintf(salida, tam, "%s_bn.%s", base, extension);
    } else {
        snprintf(salida, tam, "%s_bn", base);
    }
}

/* Convierte la imagen indicada a blanco y negro (escala de grises)
 * usando la formula de luminosidad: 0.299R + 0.587G + 0.114B */
int convertirABlancoYNegro(const char *rutaEntrada, const char *rutaSalida,
                            const char *extension) {
    int ancho, alto, canales;

    unsigned char *datos = stbi_load(rutaEntrada, &ancho, &alto, &canales, 0);
    if (datos == NULL) {
        printf("Error: no se pudo cargar la imagen '%s'\n", rutaEntrada);
        return 0;
    }

    printf("Imagen cargada: %dx%d pixeles, %d canal(es)\n", ancho, alto, canales);

    unsigned char *gris = malloc((size_t) ancho * alto);
    if (gris == NULL) {
        printf("Error: memoria insuficiente.\n");
        stbi_image_free(datos);
        return 0;
    }

    for (int p = 0; p < ancho * alto; p++) {
        unsigned char r, g, b;
        if (canales >= 3) {
            r = datos[p * canales + 0];
            g = datos[p * canales + 1];
            b = datos[p * canales + 2];
        } else {
            r = g = b = datos[p * canales + 0];
        }
        double luminosidad = 0.299 * r + 0.587 * g + 0.114 * b;
        gris[p] = (unsigned char) (luminosidad + 0.5);
    }

    int resultado = 0;
    if (strcmp(extension, "png") == 0) {
        resultado = stbi_write_png(rutaSalida, ancho, alto, 1, gris, ancho);
    } else if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0) {
        resultado = stbi_write_jpg(rutaSalida, ancho, alto, 1, gris, 90);
    } else if (strcmp(extension, "bmp") == 0) {
        resultado = stbi_write_bmp(rutaSalida, ancho, alto, 1, gris);
    }

    free(gris);
    stbi_image_free(datos);

    return resultado != 0;
}

void menuConvertirBlancoYNegro(void) {
    char lista[MAX_ARCHIVOS][MAX_NOMBRE];
    int total = listarImagenes(lista, MAX_ARCHIVOS);

    printf("\n=== Conversor de imagenes a blanco y negro ===\n\n");

    if (total == 0) {
        printf("No se encontraron archivos .png, .jpg, .jpeg o .bmp en esta carpeta.\n");
        return;
    }

    printf("Archivos de imagen encontrados en la carpeta actual:\n");
    for (int i = 0; i < total; i++) {
        printf("  %d. %s\n", i + 1, lista[i]);
    }

    int opcion;
    printf("\nSeleccione el numero del archivo a convertir (0 para cancelar): ");
    if (scanf("%d", &opcion) != 1) {
        printf("Entrada invalida.\n");
        while (getchar() != '\n'); /* limpiar buffer */
        return;
    }

    if (opcion == 0) {
        printf("Operacion cancelada.\n");
        return;
    }

    if (opcion < 1 || opcion > total) {
        printf("Opcion fuera de rango.\n");
        return;
    }

    const char *archivoSeleccionado = lista[opcion - 1];
    char extension[16];
    obtenerExtension(archivoSeleccionado, extension, sizeof(extension));

    char archivoSalida[MAX_NOMBRE];
    construirNombreSalida(archivoSeleccionado, archivoSalida, sizeof(archivoSalida));

    printf("\nConvirtiendo '%s' -> '%s' ...\n", archivoSeleccionado, archivoSalida);

    if (convertirABlancoYNegro(archivoSeleccionado, archivoSalida, extension)) {
        printf("Conversion completada con exito.\n");
    } else {
        printf("La conversion fallo.\n");
    }
}

/* ============================================================
 *  MODULO 2: IDENTIFICAR ARCHIVOS (extension vs contenido real)
 * ============================================================ */

const char *detectarTipoReal(const unsigned char *buf, size_t leidos) {
    if (leidos >= 8 &&
        buf[0] == 0x89 && buf[1] == 0x50 && buf[2] == 0x4E && buf[3] == 0x47 &&
        buf[4] == 0x0D && buf[5] == 0x0A && buf[6] == 0x1A && buf[7] == 0x0A) {
        return "png";
    }

    if (leidos >= 3 && buf[0] == 0xFF && buf[1] == 0xD8 && buf[2] == 0xFF) {
        return "jpg";
    }

    if (leidos >= 2 && buf[0] == 'B' && buf[1] == 'M') {
        return "bmp";
    }

    if (leidos >= 6 &&
        buf[0] == 'G' && buf[1] == 'I' && buf[2] == 'F' &&
        buf[3] == '8' && (buf[4] == '7' || buf[4] == '9') && buf[5] == 'a') {
        return "gif";
    }

    if (leidos >= 5 &&
        buf[0] == '%' && buf[1] == 'P' && buf[2] == 'D' && buf[3] == 'F' && buf[4] == '-') {
        return "pdf";
    }

    if (leidos >= 4 &&
        buf[0] == 'P' && buf[1] == 'K' &&
        (buf[2] == 0x03 || buf[2] == 0x05 || buf[2] == 0x07)) {
        return "zip"; /* incluye docx, xlsx, pptx, jar, etc. */
    }

    if (leidos >= 3 && buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3') {
        return "mp3";
    }
    if (leidos >= 2 && buf[0] == (unsigned char) 0xFF && (buf[1] & 0xE0) == 0xE0) {
        return "mp3";
    }

    if (leidos > 0) {
        int esTexto = 1;
        for (size_t i = 0; i < leidos; i++) {
            unsigned char c = buf[i];
            if (!(isprint(c) || c == '\n' || c == '\r' || c == '\t')) {
                esTexto = 0;
                break;
            }
        }
        if (esTexto) return "txt";
    }

    return "desconocido";
}

int sonCompatibles(const char *extension, const char *tipoReal) {
    if (strcmp(tipoReal, "jpg") == 0) {
        return strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0;
    }
    if (strcmp(tipoReal, "zip") == 0) {
        return strcmp(extension, "zip") == 0 || strcmp(extension, "docx") == 0 ||
               strcmp(extension, "xlsx") == 0 || strcmp(extension, "pptx") == 0 ||
               strcmp(extension, "jar") == 0;
    }
    return strcmp(extension, tipoReal) == 0;
}

void verificarArchivo(const char *nombreArchivo) {
    unsigned char buffer[BYTES_FIRMA];
    FILE *f = fopen(nombreArchivo, "rb");

    if (f == NULL) {
        printf("  [ERROR] No se pudo abrir '%s'\n", nombreArchivo);
        return;
    }

    size_t leidos = fread(buffer, 1, BYTES_FIRMA, f);
    fclose(f);

    char extension[16];
    obtenerExtension(nombreArchivo, extension, sizeof(extension));
    const char *tipoReal = detectarTipoReal(buffer, leidos);

    printf("  Archivo: %-30s Extension: %-6s Contenido detectado: %-12s -> ",
           nombreArchivo,
           extension[0] ? extension : "(ninguna)",
           tipoReal);

    if (strcmp(tipoReal, "desconocido") == 0) {
        printf("No se pudo determinar el tipo real.\n");
    } else if (sonCompatibles(extension, tipoReal)) {
        printf("VALIDO (la extension coincide con el contenido)\n");
    } else {
        printf("INVALIDO (la extension NO coincide con el contenido real)\n");
    }
}

void menuIdentificarArchivos(void) {
    char lista[MAX_ARCHIVOS][MAX_NOMBRE];
    int total = listarArchivos(lista, MAX_ARCHIVOS);

    printf("\n=== Identificador de archivos (extension vs contenido real) ===\n\n");

    if (total == 0) {
        printf("No se encontraron archivos en esta carpeta.\n");
        return;
    }

    printf("Archivos encontrados en la carpeta actual:\n");
    for (int i = 0; i < total; i++) {
        printf("  %d. %s\n", i + 1, lista[i]);
    }
    printf("  %d. Verificar TODOS los archivos\n", total + 1);

    int opcion;
    printf("\nSeleccione una opcion (0 para cancelar): ");
    if (scanf("%d", &opcion) != 1) {
        printf("Entrada invalida.\n");
        while (getchar() != '\n');
        return;
    }

    if (opcion == 0) {
        printf("Operacion cancelada.\n");
        return;
    }

    printf("\nResultado:\n");

    if (opcion == total + 1) {
        for (int i = 0; i < total; i++) {
            verificarArchivo(lista[i]);
        }
    } else if (opcion >= 1 && opcion <= total) {
        verificarArchivo(lista[opcion - 1]);
    } else {
        printf("Opcion fuera de rango.\n");
    }
}

/* ============================================================
 *  MENU PRINCIPAL
 * ============================================================ */

int main(void) {
    int opcion;

    do {
        printf("\n============================================\n");
        printf("            PROGRAMA UNIFICADO\n");
        printf("============================================\n");
        printf("  1. Convertir imagen a blanco y negro\n");
        printf("  2. Identificar archivos (extension vs contenido)\n");
        printf("  0. Salir\n");
        printf("============================================\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            printf("Entrada invalida.\n");
            while (getchar() != '\n'); /* limpiar buffer de entrada */
            continue;
        }

        switch (opcion) {
            case 1:
                menuConvertirBlancoYNegro();
                break;
            case 2:
                menuIdentificarArchivos();
                break;
            case 0:
                printf("\nSaliendo del programa...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }

    } while (opcion != 0);

    return 0;
}
