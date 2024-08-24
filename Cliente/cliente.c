#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9090
#define MAX_LEN 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LEN] = {0};
    char response[MAX_LEN] = {0};
    int option;
    int length;

    // Inicializar Winsock
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fallo en la inicializaci�n de Winsock. C�digo de error: %d\n", WSAGetLastError());
        return 1;
    }

    // Crear socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. C�digo de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Error al conectar con el servidor. C�digo de error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    while (1) {
        printf("Men�:\n");
        printf("1. Generar nombre de usuario\n");
        printf("2. Generar contrase�a\n");
        printf("3. Salir\n");
        printf("Elija una opci�n: ");
        scanf("%d", &option);

        if (option == 3) {
            printf("Saliendo del programa...\n");
            break; // Salir del bucle y cerrar el cliente
        }

        if (option == 1 || option == 2) {
            while (1) {
                printf("Indique la longitud: ");
                scanf("%d", &length);

                if (option == 1) {
                    if (length < 5 || length > 15) {
                        printf("Longitud inv�lida para nombre de usuario. Debe estar entre 5 y 15.\n");
                    } else {
                        snprintf(buffer, MAX_LEN, "U%d", length);
                        break; // Longitud v�lida, salir del bucle interno
                    }
                } else if (option == 2) {
                    if (length < 8 || length >= 50) {
                        printf("Longitud inv�lida para contrase�a. Debe ser al menos 8 y menos de 50.\n");
                    } else {
                        snprintf(buffer, MAX_LEN, "P%d", length);
                        break; // Longitud v�lida, salir del bucle interno
                    }
                }
            }

            // Enviar solicitud al servidor
            if (send(sock, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
                printf("Error al enviar solicitud. C�digo de error: %d\n", WSAGetLastError());
                break; // Salir del bucle en caso de error de env�o
            }
            printf("Solicitud enviada: %s\n", buffer);

            // Recibir respuesta del servidor
            int recv_len = recv(sock, response, MAX_LEN, 0);
            if (recv_len > 0) {
                response[recv_len] = '\0'; // Asegurar el null-terminator
                printf("Respuesta del servidor: %s\n", response);
            } else if (recv_len == 0) {
                printf("El servidor ha cerrado la conexi�n.\n");
                break; // Salir del bucle si el servidor cierra la conexi�n
            } else {
                printf("Error al recibir datos. C�digo de error: %d\n", WSAGetLastError());
                break; // Salir en caso de error de recepci�n
            }
        } else {
            printf("Opci�n no v�lida.\n");
        }
    }

    // Cerrar el socket
    closesocket(sock);
    WSACleanup();
    return 0;
}
