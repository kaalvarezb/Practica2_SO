#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include "Hash.c"

#define MAX_CLIENTS 32
#define BUFFER_SIZE 1024

void log_operation(char *operation, const char *ip) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%dT%H%M%S", tm);
    printf("[%s] Cliente %s %s\n", timestamp, ip, operation);

    // Variables para la busqueda del tiempo promedio de viaje
    char origen_id[10];
    char destino_id[10];
    char hora[10];
    struct timeval start, end;
	long seconds, microseconds;

    // ALmacenamiento de los valores enviados por el cliente
    char delimiter[] = ",";

    // Obtener el primer valor
    char* value = strtok(operation, delimiter);

    if (value != NULL) {
        strcpy(origen_id, value);
        
        // Obtener el segundo valor
        value = strtok(NULL, delimiter);
        if (value != NULL) {
            strcpy(destino_id, value);

            // Obtener el tercer valor
            value = strtok(NULL, delimiter);
            if (value != NULL) {
                strcpy(hora, value);
            }
        }
    }

    printf("Origen: %s\n", origen_id);
    printf("Destino: %s\n", destino_id);
    printf("Hora: %s\n", hora);

    // Busqueda de un registro
    gettimeofday(&start, NULL);//se almacena la hora de inicio
    Registro reg = buscarRegistro(atoi(origen_id), atoi(destino_id));
    if (reg.id_origen == -1 && reg.id_destino == -1 && reg.mean_travel_time == 0.0) {
        printf("NA\n");

    } else {
        if (reg.hour == atoi(hora)) {
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


}

void handle_client(int client_socket, const char *ip) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        // Process received command from the client
        buffer[bytes_received - 1] = '\0'; // Remove newline character
        log_operation(buffer, ip);

        // Send confirmation message to the client
        const char *confirmation = "Comando recibido correctamente.\n";
        send(client_socket, confirmation, strlen(confirmation), 0);
    }

    if (bytes_received == 0) {
        printf("Cliente %s desconectado.\n", ip);
    } else {
        perror("Error al recibir datos del cliente.");
    }

    close(client_socket);
}

int main() {

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_size;
    pid_t child_pid;

    // Cargar tabla hash
    init();

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor.");
        exit(1);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(12345); // Change to desired port number

    // Bind server socket to the specified address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        close(client_socket);
        perror("Error al enlazar el socket del servidor.");
        exit(1);
    }

    // Start listening for client connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Error al escuchar en el socket del servidor.");
        exit(1);
    }

    printf("Servidor en ejecución. Esperando conexiones de clientes...\n");

    while (1) {
        client_address_size = sizeof(client_address);

        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
        if (client_socket == -1) {
            perror("Error al aceptar la conexión del cliente.");
            exit(1);
        }

        // Fork a new process to handle the client
        child_pid = fork();
        if (child_pid == 0) {
            // Child process
            close(server_socket);
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
            handle_client(client_socket, client_ip);
            exit(0);
        } else if (child_pid > 0) {
            // Parent process
            close(client_socket);
        } else {
            perror("Error al crear el proceso hijo.");
            exit(1);
        }
    }

    return 0;
}
