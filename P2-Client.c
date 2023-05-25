#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define BUFFER_SIZE 1024

void print_menu() {
    printf("Bienvenido\n");
    printf("1. Ingresar origen\n");
    printf("2. Ingresar destino\n");
    printf("3. Ingresar hora\n");
    printf("4. Buscar tiempo de viaje medio\n");
    printf("5. Salir\n");
}

void send_command(int server_socket, const char *command) {
    send(server_socket, command, strlen(command), 0);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    } else {
        perror("Error al recibir datos del servidor.");
    }
}

int main() {
    int server_socket;
    struct sockaddr_in server_address;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del cliente.");
        exit(1);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("25.7.197.140"); // Change to server IP address
    server_address.sin_port = htons(12345); // Change to server port number

    // Connect to the server
    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al conectar con el servidor.");
        exit(1);
    }

    int option;
    char command[BUFFER_SIZE];

    do {
        print_menu();
        printf("Seleccione una opción: ");
        scanf("%d", &option);
        getchar(); // Consume newline character

        switch (option) {
            case 1:
                printf("Ingresar origen: ");
                fgets(command, BUFFER_SIZE, stdin);
                send_command(server_socket, command);
                break;
            case 2:
                printf("Ingresar destino: ");
                fgets(command, BUFFER_SIZE, stdin);
                send_command(server_socket, command);
                break;
            case 3:
                printf("Ingresar hora: ");
                fgets(command, BUFFER_SIZE, stdin);
                send_command(server_socket, command);
                break;
            case 4:
                printf("Buscar tiempo de viaje medio\n");
                send_command(server_socket, "Buscar tiempo de viaje medio\n");
                break;
            case 5:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción inválida. Por favor, seleccione una opción válida.\n");
                break;
        }

        printf("Presione cualquier tecla para continuar...\n");
        getchar(); // Wait for any key press
        system("clear"); // Clear the console

    } while (option != 5);

    close(server_socket);

    return 0;
}