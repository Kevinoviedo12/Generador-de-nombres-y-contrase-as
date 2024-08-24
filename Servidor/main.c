#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9090
#define MAX_LEN 1024

const char vowels[] = "aeiou";
const char consonants[] = "bcdfghjklmnpqrstvwxyz";

// Función para generar un nombre de usuario alternando entre vocales y consonantes
void generate_username(char* buffer, int length) {
    int is_vowel = rand() % 2;
    for (int i = 0; i < length; i++) {
        if (is_vowel) {
            buffer[i] = vowels[rand() % strlen(vowels)];
        } else {
            buffer[i] = consonants[rand() % strlen(consonants)];
        }
        is_vowel = !is_vowel;
    }
    buffer[length] = '\0';
}

// Función para generar una contraseña alfanumérica
void generate_password(char* buffer, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length; i++) {
        buffer[i] = charset[rand() % strlen(charset)];
    }
    buffer[length] = '\0';
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[MAX_LEN] = {0};
    char response[MAX_LEN] = {0};

    // Inicializar Winsock
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fallo en la inicialización de Winsock. Código de error: %d\n", WSAGetLastError());
        return 1;
    }

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Código de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Asociar el socket al puerto
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Error en bind. Código de error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Error en listen. Código de error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Inicializar la semilla del generador de números aleatorios
    srand((unsigned int)time(NULL));

    while (1) {
        printf("Esperando conexiones...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Error en accept. Código de error: %d\n", WSAGetLastError());
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        while (1) {
            // Recibir datos del cliente
            int recv_len = recv(new_socket, buffer, MAX_LEN, 0);
            if (recv_len > 0) {
                buffer[recv_len] = '\0';
                printf("Solicitud recibida: %s\n", buffer);

                // Procesar la solicitud
                int length = atoi(buffer + 1);
                if (buffer[0] == 'U') {  // Generar nombre de usuario
                    if (length < 5 || length > 15) {
                        strcpy(response, "Error: Longitud inválida para nombre de usuario.");
                    } else {
                        generate_username(response, length);
                    }
                } else if (buffer[0] == 'P') {  // Generar contraseña
                    if (length < 8 || length >= 50) {
                        strcpy(response, "Error: Longitud inválida para contraseña.");
                    } else {
                        generate_password(response, length);
                    }
                } else {
                    strcpy(response, "Error: Solicitud no válida.");
                }

                // Enviar respuesta al cliente
                if (send(new_socket, response, strlen(response), 0) == SOCKET_ERROR) {
                    printf("Error al enviar datos. Código de error: %d\n", WSAGetLastError());
                } else {
                    printf("Respuesta enviada: %s\n", response);
                }
            } else if (recv_len == 0) {
                // Cliente cerrado la conexión
                printf("El cliente ha cerrado la conexión.\n");
                break;
            } else {
                // Error en la recepción
                printf("Error al recibir datos. Código de error: %d\n", WSAGetLastError());
                break;
            }
        }

        // Cerrar el socket del cliente
        closesocket(new_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
