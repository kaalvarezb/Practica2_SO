#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 1160 // Tamaño de la tabla hash

// Estructura para cada registro del archivo csv
typedef struct {
    int id_origen; // ID del id_origen
    int id_destino; // ID del destino
    int hour; // Hora del dia
    float mean_travel_time; // Media del tiempo de viaje
    float sd_travel_time; // Desviacion estandar del tiempo de viaje
    float gm_travel_time; // Media geometrica del tiempo de viaje
    float gm_sd_travel_time; // Desviacion estandar geometrica del tiempo de viaje
} Registro;

// Estructura para la lista enlazada de cada ID indexado
typedef struct Nodo {
    Registro reg;
    struct Nodo* sig;
} Nodo;

Nodo* tabla[TAM_HASH]; // Puntero a la tabla hash

// Funcion hash
int hash(int id_origen) {
    return id_origen % TAM_HASH;
}

// Funcion para agregar un registro a la tabla hash
void agregarRegistro(Registro reg) {
    int indice = hash(reg.id_origen); // Calculo del indice de la tabla hash
    Nodo* nuevoNodo = (Nodo*) malloc(sizeof(Nodo));
    nuevoNodo->reg = reg;
    nuevoNodo->sig = tabla[indice];
    tabla[indice] = nuevoNodo;
}

// Funcion para buscar un registro en la tabla hash
Registro buscarRegistro(int id_origen, int destino) {
    int indice = hash(id_origen); // Calculo del indice de la tabla hash
    Nodo* actual = tabla[indice];
    while (actual != NULL) {
        if (actual->reg.id_origen == id_origen && actual->reg.id_destino == destino) {
            return actual->reg;
        }
        actual = actual->sig;
    }
    Registro regInvalido = {-1, -1, -1.0}; // registro inválido para indicar que no se encontró el registro buscado
    return regInvalido;
}

// Guardar la tabla hash en un archivo binario
void guardarTabla(char* hashTable) {
    FILE* archivo = fopen(hashTable, "wb");
    for (int i = 0; i < TAM_HASH; i++) {
        Nodo* actual = tabla[i];
        while (actual != NULL) {
            fwrite(&(actual->reg), sizeof(Registro), 1, archivo);
            actual = actual->sig;
        }
    }
    fclose(archivo);
}

// Abrir el archivobinario de la tabla hash
void cargarTabla(char* hashTable) {
    FILE* archivo = fopen(hashTable, "rb");
    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, archivo) == 1) {
        agregarRegistro(reg);
    }
    fclose(archivo);
}

int init() {
    
    // verificar si el archivo binario de la tabla hash existe
    FILE* archivo = fopen("HashTable.bin", "rb");
    if (archivo != NULL) {
        fclose(archivo);
        // Cargar tabla hash desde archivo binario
        for (int i = 0; i < TAM_HASH; i++) {
            tabla[i] = NULL;
        }
        cargarTabla("HashTable.bin");
    }
    else {
        // Cargar registros desde archivo CSV y agregarlos a la tabla hash
        FILE* archivoCSV = fopen("bogota-cadastral-2019-3-All-HourlyAggregate.csv", "r");
        if (archivoCSV == NULL) {
            printf("Error al abrir el archivo CSV\n");
            return 1;
        }
        char linea[100];
        while (fgets(linea, 100, archivoCSV) != NULL) {
            Registro reg;
            sscanf(linea, "%d,%d,%d,%f", &(reg.id_origen), &(reg.id_destino), &(reg.hour), &(reg.mean_travel_time));
            agregarRegistro(reg);
        }
        fclose(archivoCSV);

        // Guardar tabla hash en archivo binario
        guardarTabla("HashTable.bin");

    }

}


