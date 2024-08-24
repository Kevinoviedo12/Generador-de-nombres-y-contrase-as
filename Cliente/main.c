#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Vincular la biblioteca Winsock

#define PORT 8080
#define MAX_LEN 1024

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LEN] = {0};
    char response[MAX_LEN] = {0};
    char option;
    int length;

    // Inicializar Winsock
    printf("\nInicializando Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fallo en la inicialización de Winsock. Error Código: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Inicializado.\n");

    // Crear socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Error Código: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nError al conectar con el servidor. Código: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Menú:\n");
    printf("1. Generar nombre de usuario\n");
    printf("2. Generar contraseña\n");
    printf("Elija una opción: ");
    scanf(" %c", &option);
    printf("Indique la longitud: ");
    scanf("%d", &length);

    if (option == '1') {
        snprintf(buffer, MAX_LEN, "U%d", length);
    } else if (option == '2') {
        snprintf(buffer, MAX_LEN, "P%d", length);
    } else {
        printf("Opción no válida.\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Enviar solicitud al servidor
    send(sock, buffer, strlen(buffer), 0);
    printf("Solicitud enviada: %s\n", buffer);

    // Recibir respuesta del servidor
    int recv_len = recv(sock, response, MAX_LEN, 0);
    if (recv_len > 0) {
        response[recv_len] = '\0';  // Asegurar el null-terminator
        printf("Respuesta del servidor: %s\n", response);
    } else {
        printf("Error al recibir datos. Código: %d\n", WSAGetLastError());
    }

    // Cerrar el socket
    closesocket(sock);
    WSACleanup();
    return 0;
}
