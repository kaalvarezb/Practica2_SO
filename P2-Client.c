#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include "P2-Server.c"

#define BUFFER_SIZE 25
#define MAX_ZONAS 10000 // Máximo número de zonas en el archivo csv

// Estructura que contiene los campos del archivo csv
typedef struct {
    int sourceid;
    int dstid;
    int hod;
    float mean_travel_time;
    float standard_deviation_travel_time;
    float geometric_mean_travel_time;
    float geometric_standard_deviation_travel_time;
} Viaje;

int main() {

    int fd[2]; // Descriptores de archivo para la tubería
    pid_t pid; // Identificadores de procesos hijos
    char buffer[BUFFER_SIZE]; // Buffer para leer los datos de la tubería

    // Crear el archivo FIFO
    unlink("/tmp/myfifo");
    if (mkfifo("/tmp/myfifo", 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Interfaz usuario

    int opcion;
    int origen_id;
    int destino_id;
    int hora;
    int* hora_ptr;
    int* origen_id_ptr;
    int* destino_id_ptr;
    float tiempo_viaje_medio;
    struct timeval start, end;
	long seconds, microseconds;

    printf("\nBienvenido\n");
    while (opcion != 5) {
        printf("1. Ingresar origen\n");
        printf("2. Ingresar destino\n");
        printf("3. Ingresar hora\n");
        printf("4. Buscar tiempo de viaje medio\n");
        printf("5. Salir\n");
        printf("Ingrese su opcion y presione enter: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                origen_id_ptr = &origen_id;
                do {
                    printf("Ingrese ID del origen: ");
                    scanf("%d", origen_id_ptr);
                    if (*origen_id_ptr < 1 || *origen_id_ptr > 1160) {
                        printf("ID invalido, intente de nuevo\n");
                    }
                } while (*origen_id_ptr < 1 || *origen_id_ptr > 1160);
                origen_id = *origen_id_ptr;
                break;
            case 2:
                destino_id_ptr = &destino_id;
                do {
                    printf("Ingrese ID del destino: ");
                    scanf("%d", destino_id_ptr);
                    if (*destino_id_ptr < 1 || *destino_id_ptr > 1160) {
                        printf("ID invalido, intente de nuevo\n");
                    }
                } while (*destino_id_ptr < 1 || *destino_id_ptr > 1160);
                destino_id = *destino_id_ptr;
                break;
            case 3:
                hora_ptr = &hora;
                do {
                    printf("Ingrese hora del dia: ");
                    scanf("%d", &hora);
                    if (*hora_ptr < 0 || *hora_ptr > 23) {
                        printf("Hora invalida, intente de nuevo\n");
                    }
                } while (*hora_ptr < 0 || *hora_ptr > 23);
                hora = *hora_ptr;
                break;
            case 4:
                // Crear proceso hijo
                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (pid == 0) {
                    // Proceso hijo
                    char str_or[10];
                    char str_des[10];
                    char str_hor[10];
                    int or;
                    int des;
                    int hor;

                    // Leer de la tubería los datos ingresados por el usuario
                    fd[0] = open("/tmp/myfifo", O_RDONLY);
                    read(fd[0], str_or, sizeof(str_or));
                    or = atoi(str_or);
                    read(fd[0], str_des, sizeof(str_des));
                    des = atoi(str_des);
                    read(fd[0], str_hor, sizeof(str_hor));
                    hor = atoi(str_hor);
                    close(fd[0]);


                    // Cargar tabla hash
                    init();
                    // Busqueda de un registro
                    gettimeofday(&start, NULL);//se almacena la hora de inicio
                    Registro reg = buscarRegistro(or, des);
                    if (reg.id_origen == -1 && reg.id_destino == -1 && reg.mean_travel_time == 0.0) {
                        printf("NA\n");

                    } else {
                        if (reg.hour == hor) {
                            printf("Registro encontrado: origen = %d, destino = %d, hora = %d, tiempo = %.2f\n", reg.id_origen, reg.id_destino, reg.hour, reg.mean_travel_time);
                        }
                        else {
                            printf("La hora no coincide con los registros\n");
                        }
                    }
                    gettimeofday(&end, NULL);// se almacena la hora de finalizacion
                    seconds = end.tv_sec - start.tv_sec;
                    microseconds = end.tv_usec - start.tv_usec;
                    printf("Tiempo de la busqueda: %d.%0.6d segundos \n", seconds, microseconds); // se imprime el tiempo que toma la busqueda

                    exit(EXIT_SUCCESS);

                } else {
                    // Proceso padre
                    char str_or[10];
                    char str_des[10];
                    char str_hor[10];

                    if (origen_id_ptr != NULL){
                        // Convertir el entero a una cadena de caracteres
                        snprintf(str_or, sizeof(str_or), "%d", *origen_id_ptr);
                    } else {
                        printf("Debe ingresar el origen\n");
                        break;
                    }

                    if (destino_id_ptr != NULL){
                        // Convertir el entero a una cadena de caracteres
                        snprintf(str_des, sizeof(str_des), "%d", *destino_id_ptr);
                    } else {
                        printf("Debe ingresar el destino\n");
                        break;
                    }

                    if (hora_ptr != NULL){
                        // Convertir el entero a una cadena de caracteres
                        snprintf(str_hor, sizeof(str_hor), "%d", *hora_ptr);
                    } else {
                        printf("Debe ingresar la hora\n");
                        break;
                    }
                    

                    // Enviar a la tubería los datos ingresados por el usuario
                    fd[1] = open("/tmp/myfifo", O_WRONLY);
                    write(fd[1], str_or, sizeof(str_or));
                    write(fd[1], str_des, sizeof(str_des));
                    write(fd[1], str_hor, sizeof(str_hor));
                    close(fd[1]);
                    wait(NULL);
                }
                
                break;
            case 5:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opcion invalida, intente de nuevo\n");
                break;
        }
    }
    // Eliminar el archivo FIFO
    unlink("/tmp/myfifo");
    
    return 0;
}
