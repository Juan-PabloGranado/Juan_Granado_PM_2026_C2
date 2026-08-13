/* ============================================================================
   PROGRAMA: Calculadora de Costo Real de Vehículos por Kilómetro
   ============================================================================
   Este programa permite:
     1. Crear vehículos con sus parámetros y guardarlos en un archivo CSV.
     2. Borrar un vehículo del archivo.
     3. Modificar los parámetros de un vehículo existente.
     4. Calcular el costo real de un viaje (según km en ciudad y autopista).
     5. Modificar el precio global del combustible.
     6. Listar los vehículos guardados y ver el desglose de costo por km.

   MODELO DE CÁLCULO
   ------------------
   El costo real de recorrer un kilómetro con un vehículo se compone de
   costos FIJOS (no dependen del tipo de vía, se reparten entre todos los
   km que el vehículo recorrerá en su vida útil) y costos VARIABLES
   (el combustible, que sí depende de si se conduce en ciudad o autopista).

   Costos fijos por km = amortización/km + mantenimiento/km + seguro/km
                          + neumáticos/km

   Costo por km en ciudad     = costos fijos por km + combustible/km (ciudad)
   Costo por km en autopista  = costos fijos por km + combustible/km (autopista)

   ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_VEHICULOS "vehiculos.csv"
#define ARCHIVO_CONFIG    "config.txt"
#define MAX_NOMBRE 50

/* ---------------------------------------------------------------------- */
/* Estructura que representa un vehículo y todos los parámetros que      */
/* el usuario necesita ingresar para poder calcular sus costos.          */
/* ---------------------------------------------------------------------- */
typedef struct {
    int    id;
    char   nombre[MAX_NOMBRE];
    double costo;                 /* Costo de compra del vehículo            */
    double valor_residual;        /* Valor de reventa al final de vida útil  */
    int    vida_util_anios;       /* Años de vida útil                       */
    double km_anual_ciudad;       /* Km promedio recorridos al año en ciudad */
    double km_anual_autopista;    /* Km promedio recorridos al año en autop. */
    double consumo_ciudad;        /* Rendimiento en ciudad (km por litro)    */
    double consumo_autopista;     /* Rendimiento en autopista (km por litro) */
    double seguro_anual;          /* Costo anual del seguro                  */
    double mantenimiento_anual;   /* Costo anual promedio de mantenimiento   */
    double costo_neumaticos;      /* Costo de un juego de neumáticos         */
    double km_vida_neumaticos;    /* Km que dura un juego de neumáticos      */
} Vehiculo;

/* ---------------------------------------------------------------------- */
/* Prototipos                                                             */
/* ---------------------------------------------------------------------- */
int    cargarVehiculos(Vehiculo **lista);
void   guardarVehiculos(Vehiculo *lista, int n);
double leerPrecioCombustible(void);
void   guardarPrecioCombustible(double precio);

void   crearVehiculo(void);
void   borrarVehiculo(void);
void   modificarVehiculo(void);
void   listarVehiculos(void);
void   calcularViaje(void);
void   cambiarPrecioCombustible(void);
void   verDesgloseCostoPorKm(void);

double amortizacionAnual(Vehiculo v);
double kmAnualTotal(Vehiculo v);
double kmVidaUtilTotal(Vehiculo v);
double amortizacionPorKm(Vehiculo v);
double mantenimientoPorKm(Vehiculo v);
double seguroPorKm(Vehiculo v);
double neumaticosPorKm(Vehiculo v);
double costosFijosPorKm(Vehiculo v);
double combustiblePorKm(Vehiculo v, double consumo, double precioCombustible);
double costoPorKmCiudad(Vehiculo v, double precioCombustible);
double costoPorKmAutopista(Vehiculo v, double precioCombustible);

int    buscarIndicePorId(Vehiculo *lista, int n, int id);
int    siguienteId(Vehiculo *lista, int n);
void   limpiarBuffer(void);
void   mostrarVehiculo(Vehiculo v);

/* ---------------------------------------------------------------------- */
/* main: menú principal                                                   */
/* ---------------------------------------------------------------------- */
int main(void) {
    int opcion;

    do {
        printf("\n============================================\n");
        printf("   COSTO REAL DE VEHICULOS POR KM\n");
        printf("============================================\n");
        printf("1. Crear vehiculo\n");
        printf("2. Borrar vehiculo\n");
        printf("3. Modificar parametros de un vehiculo\n");
        printf("4. Calcular costo de un viaje\n");
        printf("5. Modificar precio del combustible\n");
        printf("6. Listar vehiculos\n");
        printf("7. Ver desglose de costo por km de un vehiculo\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = -1;
            printf("Entrada invalida.\n");
            continue;
        }
        limpiarBuffer();

        switch (opcion) {
            case 1: crearVehiculo(); break;
            case 2: borrarVehiculo(); break;
            case 3: modificarVehiculo(); break;
            case 4: calcularViaje(); break;
            case 5: cambiarPrecioCombustible(); break;
            case 6: listarVehiculos(); break;
            case 7: verDesgloseCostoPorKm(); break;
            case 0: printf("Hasta luego.\n"); break;
            default: printf("Opcion no valida.\n");
        }
    } while (opcion != 0);

    return 0;
}

/* ---------------------------------------------------------------------- */
/* Limpia el buffer de entrada tras un scanf                              */
/* ---------------------------------------------------------------------- */
void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
}

/* ========================================================================
   MANEJO DE ARCHIVO (persistencia de vehículos en CSV)
   ======================================================================== */

/* Carga todos los vehiculos del archivo CSV en un arreglo dinamico.
   Devuelve la cantidad de vehiculos leidos y deja el puntero *lista
   apuntando al arreglo (el llamador es responsable de hacer free()). */
int cargarVehiculos(Vehiculo **lista) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "r");
    int capacidad = 10;
    int n = 0;
    Vehiculo *arr = malloc(capacidad * sizeof(Vehiculo));
    if (arr == NULL) {
        printf("Error de memoria.\n");
        exit(1);
    }

    if (f == NULL) {
        /* Aun no existe el archivo: no hay vehiculos guardados */
        *lista = arr;
        return 0;
    }

    char linea[512];
    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '\n' || linea[0] == '\0') continue;

        if (n >= capacidad) {
            capacidad *= 2;
            Vehiculo *tmp = realloc(arr, capacidad * sizeof(Vehiculo));
            if (tmp == NULL) {
                printf("Error de memoria.\n");
                fclose(f);
                free(arr);
                exit(1);
            }
            arr = tmp;
        }

        Vehiculo v;
        int leidos = sscanf(linea,
            "%d,%49[^,],%lf,%lf,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
            &v.id, v.nombre, &v.costo, &v.valor_residual, &v.vida_util_anios,
            &v.km_anual_ciudad, &v.km_anual_autopista,
            &v.consumo_ciudad, &v.consumo_autopista,
            &v.seguro_anual, &v.mantenimiento_anual,
            &v.costo_neumaticos, &v.km_vida_neumaticos);

        if (leidos == 13) {
            arr[n] = v;
            n++;
        }
    }

    fclose(f);
    *lista = arr;
    return n;
}

/* Sobrescribe el archivo CSV con el contenido completo del arreglo.
   Se usa despues de crear, borrar o modificar un vehiculo. */
void guardarVehiculos(Vehiculo *lista, int n) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "w");
    if (f == NULL) {
        printf("No se pudo escribir el archivo de vehiculos.\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        Vehiculo v = lista[i];
        fprintf(f, "%d,%s,%.2f,%.2f,%d,%.2f,%.2f,%.4f,%.4f,%.2f,%.2f,%.2f,%.2f\n",
            v.id, v.nombre, v.costo, v.valor_residual, v.vida_util_anios,
            v.km_anual_ciudad, v.km_anual_autopista,
            v.consumo_ciudad, v.consumo_autopista,
            v.seguro_anual, v.mantenimiento_anual,
            v.costo_neumaticos, v.km_vida_neumaticos);
    }
    fclose(f);
}

/* Lee el precio del combustible desde config.txt.
   Si el archivo no existe, se usa 250.00 (RD$/litro) por defecto. */
double leerPrecioCombustible(void) {
    FILE *f = fopen(ARCHIVO_CONFIG, "r");
    double precio = 250.00;
    if (f != NULL) {
        if (fscanf(f, "%lf", &precio) != 1) {
            precio = 250.00;
        }
        fclose(f);
    }
    return precio;
}

void guardarPrecioCombustible(double precio) {
    FILE *f = fopen(ARCHIVO_CONFIG, "w");
    if (f == NULL) {
        printf("No se pudo guardar el precio del combustible.\n");
        return;
    }
    fprintf(f, "%.2f\n", precio);
    fclose(f);
}

/* ========================================================================
   FUNCIONES DE CALCULO
   ======================================================================== */

/* 1. Amortizacion anual: reparte (costo - valor residual) entre los
      anios de vida util del vehiculo. */
double amortizacionAnual(Vehiculo v) {
    if (v.vida_util_anios <= 0) return 0;
    return (v.costo - v.valor_residual) / v.vida_util_anios;
}

double kmAnualTotal(Vehiculo v) {
    return v.km_anual_ciudad + v.km_anual_autopista;
}

/* Total de km que recorrera el vehiculo durante toda su vida util */
double kmVidaUtilTotal(Vehiculo v) {
    return kmAnualTotal(v) * v.vida_util_anios;
}

/* Amortizacion repartida en cada km recorrido durante la vida util */
double amortizacionPorKm(Vehiculo v) {
    double kmTotal = kmVidaUtilTotal(v);
    if (kmTotal <= 0) return 0;
    return (v.costo - v.valor_residual) / kmTotal;
}

/* 2. Gasto de mantenimiento por km: el mantenimiento total durante la
      vida util (mantenimiento_anual * anios) se divide entre el total
      de km recorridos en esa vida util. */
double mantenimientoPorKm(Vehiculo v) {
    double kmTotal = kmVidaUtilTotal(v);
    if (kmTotal <= 0) return 0;
    double mantenimientoTotal = v.mantenimiento_anual * v.vida_util_anios;
    return mantenimientoTotal / kmTotal;
}

/* Seguro por km: el seguro anual se reparte entre los km recorridos ese anio */
double seguroPorKm(Vehiculo v) {
    double kmAnual = kmAnualTotal(v);
    if (kmAnual <= 0) return 0;
    return v.seguro_anual / kmAnual;
}

/* Gasto de neumaticos por km: costo de un juego / km que dura ese juego */
double neumaticosPorKm(Vehiculo v) {
    if (v.km_vida_neumaticos <= 0) return 0;
    return v.costo_neumaticos / v.km_vida_neumaticos;
}

/* Suma de todos los costos FIJOS por km (no dependen de ciudad/autopista) */
double costosFijosPorKm(Vehiculo v) {
    return amortizacionPorKm(v) + mantenimientoPorKm(v)
         + seguroPorKm(v) + neumaticosPorKm(v);
}

/* 3. Gasto de combustible por km, segun el rendimiento (km/l) dado
      y el precio del combustible por litro. */
double combustiblePorKm(Vehiculo v, double consumo, double precioCombustible) {
    (void)v;
    if (consumo <= 0) return 0;
    return precioCombustible / consumo;
}

/* 4. Costo del vehiculo por km, en ciudad y en autopista */
double costoPorKmCiudad(Vehiculo v, double precioCombustible) {
    return costosFijosPorKm(v) + combustiblePorKm(v, v.consumo_ciudad, precioCombustible);
}

double costoPorKmAutopista(Vehiculo v, double precioCombustible) {
    return costosFijosPorKm(v) + combustiblePorKm(v, v.consumo_autopista, precioCombustible);
}

/* ========================================================================
   UTILIDADES
   ======================================================================== */

int buscarIndicePorId(Vehiculo *lista, int n, int id) {
    for (int i = 0; i < n; i++) {
        if (lista[i].id == id) return i;
    }
    return -1;
}

int siguienteId(Vehiculo *lista, int n) {
    int maxId = 0;
    for (int i = 0; i < n; i++) {
        if (lista[i].id > maxId) maxId = lista[i].id;
    }
    return maxId + 1;
}

void mostrarVehiculo(Vehiculo v) {
    printf("  ID: %d | Nombre: %s\n", v.id, v.nombre);
    printf("  Costo: %.2f | Valor residual: %.2f | Vida util: %d anios\n",
           v.costo, v.valor_residual, v.vida_util_anios);
    printf("  Km/anio ciudad: %.0f | Km/anio autopista: %.0f\n",
           v.km_anual_ciudad, v.km_anual_autopista);
    printf("  Consumo ciudad: %.2f km/l | Consumo autopista: %.2f km/l\n",
           v.consumo_ciudad, v.consumo_autopista);
    printf("  Seguro anual: %.2f | Mantenimiento anual: %.2f\n",
           v.seguro_anual, v.mantenimiento_anual);
    printf("  Neumaticos: %.2f cada %.0f km\n",
           v.costo_neumaticos, v.km_vida_neumaticos);
}

/* ========================================================================
   OPCIONES DEL MENU
   ======================================================================== */

/* 2.1 Crear vehiculo con sus parametros y guardarlos en el archivo */
void crearVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    Vehiculo v;
    v.id = siguienteId(lista, n);

    printf("\n--- Crear vehiculo (ID asignado: %d) ---\n", v.id);

    printf("Nombre/modelo: ");
    fgets(v.nombre, MAX_NOMBRE, stdin);
    v.nombre[strcspn(v.nombre, "\n")] = '\0';

    printf("Costo de compra: ");
    scanf("%lf", &v.costo);
    printf("Valor residual al final de la vida util: ");
    scanf("%lf", &v.valor_residual);
    printf("Vida util (anios): ");
    scanf("%d", &v.vida_util_anios);
    printf("Km promedio recorridos por anio en CIUDAD: ");
    scanf("%lf", &v.km_anual_ciudad);
    printf("Km promedio recorridos por anio en AUTOPISTA: ");
    scanf("%lf", &v.km_anual_autopista);
    printf("Consumo en ciudad (km por litro): ");
    scanf("%lf", &v.consumo_ciudad);
    printf("Consumo en autopista (km por litro): ");
    scanf("%lf", &v.consumo_autopista);
    printf("Seguro anual: ");
    scanf("%lf", &v.seguro_anual);
    printf("Mantenimiento anual promedio: ");
    scanf("%lf", &v.mantenimiento_anual);
    printf("Costo de un juego de neumaticos: ");
    scanf("%lf", &v.costo_neumaticos);
    printf("Km de duracion de un juego de neumaticos: ");
    scanf("%lf", &v.km_vida_neumaticos);
    limpiarBuffer();

    /* Ampliar el arreglo en memoria y agregar el nuevo vehiculo */
    Vehiculo *tmp = realloc(lista, (n + 1) * sizeof(Vehiculo));
    if (tmp == NULL) {
        printf("Error de memoria.\n");
        free(lista);
        return;
    }
    lista = tmp;
    lista[n] = v;
    n++;

    guardarVehiculos(lista, n);
    printf("Vehiculo '%s' guardado con ID %d.\n", v.nombre, v.id);

    free(lista);
}

/* 2.2 Borrar vehiculo */
void borrarVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    if (n == 0) {
        printf("No hay vehiculos guardados.\n");
        free(lista);
        return;
    }

    listarVehiculos();
    int id;
    printf("Ingrese el ID del vehiculo a borrar: ");
    scanf("%d", &id);
    limpiarBuffer();

    int idx = buscarIndicePorId(lista, n, id);
    if (idx == -1) {
        printf("No existe un vehiculo con ese ID.\n");
        free(lista);
        return;
    }

    printf("Se eliminara: %s. Confirmar? (s/n): ", lista[idx].nombre);
    char resp = getchar();
    limpiarBuffer();
    if (resp != 's' && resp != 'S') {
        printf("Operacion cancelada.\n");
        free(lista);
        return;
    }

    /* Desplazar los elementos posteriores una posicion hacia atras */
    for (int i = idx; i < n - 1; i++) {
        lista[i] = lista[i + 1];
    }
    n--;

    guardarVehiculos(lista, n);
    printf("Vehiculo eliminado.\n");

    free(lista);
}

/* 2.3 Modificar los parametros de un vehiculo */
void modificarVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    if (n == 0) {
        printf("No hay vehiculos guardados.\n");
        free(lista);
        return;
    }

    listarVehiculos();
    int id;
    printf("Ingrese el ID del vehiculo a modificar: ");
    scanf("%d", &id);
    limpiarBuffer();

    int idx = buscarIndicePorId(lista, n, id);
    if (idx == -1) {
        printf("No existe un vehiculo con ese ID.\n");
        free(lista);
        return;
    }

    Vehiculo *v = &lista[idx];
    int campo;
    do {
        printf("\n--- Modificando: %s (ID %d) ---\n", v->nombre, v->id);
        printf("1. Nombre (%s)\n", v->nombre);
        printf("2. Costo (%.2f)\n", v->costo);
        printf("3. Valor residual (%.2f)\n", v->valor_residual);
        printf("4. Vida util anios (%d)\n", v->vida_util_anios);
        printf("5. Km/anio ciudad (%.2f)\n", v->km_anual_ciudad);
        printf("6. Km/anio autopista (%.2f)\n", v->km_anual_autopista);
        printf("7. Consumo ciudad km/l (%.2f)\n", v->consumo_ciudad);
        printf("8. Consumo autopista km/l (%.2f)\n", v->consumo_autopista);
        printf("9. Seguro anual (%.2f)\n", v->seguro_anual);
        printf("10. Mantenimiento anual (%.2f)\n", v->mantenimiento_anual);
        printf("11. Costo neumaticos (%.2f)\n", v->costo_neumaticos);
        printf("12. Km vida neumaticos (%.2f)\n", v->km_vida_neumaticos);
        printf("0. Terminar y guardar\n");
        printf("Campo a modificar: ");
        scanf("%d", &campo);
        limpiarBuffer();

        switch (campo) {
            case 1:
                printf("Nuevo nombre: ");
                fgets(v->nombre, MAX_NOMBRE, stdin);
                v->nombre[strcspn(v->nombre, "\n")] = '\0';
                break;
            case 2: printf("Nuevo costo: "); scanf("%lf", &v->costo); limpiarBuffer(); break;
            case 3: printf("Nuevo valor residual: "); scanf("%lf", &v->valor_residual); limpiarBuffer(); break;
            case 4: printf("Nueva vida util (anios): "); scanf("%d", &v->vida_util_anios); limpiarBuffer(); break;
            case 5: printf("Nuevo km/anio ciudad: "); scanf("%lf", &v->km_anual_ciudad); limpiarBuffer(); break;
            case 6: printf("Nuevo km/anio autopista: "); scanf("%lf", &v->km_anual_autopista); limpiarBuffer(); break;
            case 7: printf("Nuevo consumo ciudad: "); scanf("%lf", &v->consumo_ciudad); limpiarBuffer(); break;
            case 8: printf("Nuevo consumo autopista: "); scanf("%lf", &v->consumo_autopista); limpiarBuffer(); break;
            case 9: printf("Nuevo seguro anual: "); scanf("%lf", &v->seguro_anual); limpiarBuffer(); break;
            case 10: printf("Nuevo mantenimiento anual: "); scanf("%lf", &v->mantenimiento_anual); limpiarBuffer(); break;
            case 11: printf("Nuevo costo neumaticos: "); scanf("%lf", &v->costo_neumaticos); limpiarBuffer(); break;
            case 12: printf("Nuevo km vida neumaticos: "); scanf("%lf", &v->km_vida_neumaticos); limpiarBuffer(); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
    } while (campo != 0);

    guardarVehiculos(lista, n);
    printf("Vehiculo actualizado correctamente.\n");

    free(lista);
}

/* Lista todos los vehiculos guardados (resumen) */
void listarVehiculos(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    printf("\n--- Vehiculos guardados ---\n");
    if (n == 0) {
        printf("(no hay vehiculos registrados)\n");
    } else {
        for (int i = 0; i < n; i++) {
            printf("[%d] %s - Costo: %.2f - Vida util: %d anios\n",
                   lista[i].id, lista[i].nombre, lista[i].costo,
                   lista[i].vida_util_anios);
        }
    }
    free(lista);
}

/* 2.5 Modificar precio del combustible (parametro global) */
void cambiarPrecioCombustible(void) {
    double actual = leerPrecioCombustible();
    printf("Precio actual del combustible: %.2f por litro\n", actual);
    printf("Nuevo precio: ");
    double nuevo;
    scanf("%lf", &nuevo);
    limpiarBuffer();

    if (nuevo <= 0) {
        printf("El precio debe ser mayor que cero. No se realizaron cambios.\n");
        return;
    }

    guardarPrecioCombustible(nuevo);
    printf("Precio del combustible actualizado a %.2f por litro.\n", nuevo);
}

/* Muestra el desglose completo de costo por km de un vehiculo:
   amortizacion, mantenimiento, seguro, neumaticos, combustible,
   y el costo total por km en ciudad y en autopista. */
void verDesgloseCostoPorKm(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    if (n == 0) {
        printf("No hay vehiculos guardados.\n");
        free(lista);
        return;
    }

    listarVehiculos();
    int id;
    printf("Ingrese el ID del vehiculo: ");
    scanf("%d", &id);
    limpiarBuffer();

    int idx = buscarIndicePorId(lista, n, id);
    if (idx == -1) {
        printf("No existe un vehiculo con ese ID.\n");
        free(lista);
        return;
    }

    Vehiculo v = lista[idx];
    double precioCombustible = leerPrecioCombustible();

    printf("\n=== Desglose de costo por km: %s ===\n", v.nombre);
    printf("Precio de combustible usado: %.2f por litro\n\n", precioCombustible);
    printf("Amortizacion por km:     %.4f\n", amortizacionPorKm(v));
    printf("Mantenimiento por km:    %.4f\n", mantenimientoPorKm(v));
    printf("Seguro por km:           %.4f\n", seguroPorKm(v));
    printf("Neumaticos por km:       %.4f\n", neumaticosPorKm(v));
    printf("--------------------------------------\n");
    printf("Subtotal costos fijos:   %.4f\n\n", costosFijosPorKm(v));
    printf("Combustible por km ciudad:     %.4f\n",
           combustiblePorKm(v, v.consumo_ciudad, precioCombustible));
    printf("Combustible por km autopista:  %.4f\n\n",
           combustiblePorKm(v, v.consumo_autopista, precioCombustible));
    printf("COSTO REAL POR KM EN CIUDAD:      %.4f\n", costoPorKmCiudad(v, precioCombustible));
    printf("COSTO REAL POR KM EN AUTOPISTA:   %.4f\n", costoPorKmAutopista(v, precioCombustible));

    free(lista);
}

/* 2.4 Calcular viaje: dado un vehiculo, km en ciudad y km en autopista
   de un viaje especifico, calcula el gasto de combustible del viaje
   y el costo real total (incluyendo amortizacion, mantenimiento,
   seguro y neumaticos prorrateados). */
void calcularViaje(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);

    if (n == 0) {
        printf("No hay vehiculos guardados.\n");
        free(lista);
        return;
    }

    listarVehiculos();
    int id;
    printf("Ingrese el ID del vehiculo para el viaje: ");
    scanf("%d", &id);
    limpiarBuffer();

    int idx = buscarIndicePorId(lista, n, id);
    if (idx == -1) {
        printf("No existe un vehiculo con ese ID.\n");
        free(lista);
        return;
    }

    Vehiculo v = lista[idx];
    double kmCiudad, kmAutopista;

    printf("Km del viaje en CIUDAD: ");
    scanf("%lf", &kmCiudad);
    printf("Km del viaje en AUTOPISTA: ");
    scanf("%lf", &kmAutopista);
    limpiarBuffer();

    double precioCombustible = leerPrecioCombustible();

    /* 8. Costo en combustible del viaje */
    double combustibleCiudad = kmCiudad * combustiblePorKm(v, v.consumo_ciudad, precioCombustible);
    double combustibleAutopista = kmAutopista * combustiblePorKm(v, v.consumo_autopista, precioCombustible);
    double combustibleTotal = combustibleCiudad + combustibleAutopista;

    /* 5. Costo real total del viaje (fijos + combustible) */
    double costoRealCiudad = kmCiudad * costoPorKmCiudad(v, precioCombustible);
    double costoRealAutopista = kmAutopista * costoPorKmAutopista(v, precioCombustible);
    double costoRealTotal = costoRealCiudad + costoRealAutopista;

    double kmTotalViaje = kmCiudad + kmAutopista;
    double costoPromedioPorKm = (kmTotalViaje > 0) ? costoRealTotal / kmTotalViaje : 0;

    printf("\n=== Resultado del viaje: %s ===\n", v.nombre);
    printf("Km en ciudad: %.2f | Km en autopista: %.2f | Km totales: %.2f\n",
           kmCiudad, kmAutopista, kmTotalViaje);
    printf("Precio de combustible: %.2f por litro\n\n", precioCombustible);

    printf("--- Gasto en combustible ---\n");
    printf("Combustible en ciudad:      %.2f\n", combustibleCiudad);
    printf("Combustible en autopista:   %.2f\n", combustibleAutopista);
    printf("TOTAL COMBUSTIBLE:          %.2f\n\n", combustibleTotal);

    printf("--- Costo real total (incluye amortizacion, mantenimiento,\n");
    printf("    seguro, neumaticos y combustible) ---\n");
    printf("Costo real en ciudad:       %.2f\n", costoRealCiudad);
    printf("Costo real en autopista:    %.2f\n", costoRealAutopista);
    printf("COSTO REAL TOTAL DEL VIAJE: %.2f\n", costoRealTotal);
    printf("Costo promedio por km:      %.4f\n", costoPromedioPorKm);

    free(lista);
}
