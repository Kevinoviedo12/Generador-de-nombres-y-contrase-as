#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 9090
#define MAX_LONG 1024

const char vocales[] = "aeiou";
const char consonantes[] = "bcdfghjklmnpqrstvwxyz";

// Hice la funcion para generar un nombre de usuario entre vocales y consonantes
void generar_nombre_usuario(char* buffer, int longitud) {
    int es_vocal = rand() % 2;
    for (int i = 0; i < longitud; i++) {
        if (es_vocal) {
            buffer[i] = vocales[rand() % strlen(vocales)];
        } else {
            buffer[i] = consonantes[rand() % strlen(consonantes)];
        }
        es_vocal = !es_vocal;
    }
    buffer[longitud] = '\0';
}

// Realizo la funcion generar una contraseña
void generar_contrasena(char* buffer, int longitud) {
    const char conjunto[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < longitud; i++) {
        buffer[i] = conjunto[rand() % strlen(conjunto)];
    }
    buffer[longitud] = '\0';
}

int main() {
    WSADATA wsa;
    SOCKET servidor_fd, nuevo_socket;
    struct sockaddr_in direccion;
    int longitud_direccion = sizeof(direccion);
    char buffer[MAX_LONG] = {0};
    char respuesta[MAX_LONG] = {0};

    // Inicializar Winsock
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Fallo en la inicialización de Winsock. Código de error: %d\n", WSAGetLastError());
        return 1;
    }

    // Creo el socket
    if ((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Codigo de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PUERTO);

    // Realizo el enlace entre el socket al puerto
    if (bind(servidor_fd, (struct sockaddr*)&direccion, sizeof(direccion)) == SOCKET_ERROR) {
        printf("Error en bind. Codigo de error: %d\n", WSAGetLastError());
        closesocket(servidor_fd);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones
    if (listen(servidor_fd, 3) == SOCKET_ERROR) {
        printf("Error en listen. Código de error: %d\n", WSAGetLastError());
        closesocket(servidor_fd);
        WSACleanup();
        return 1;
    }

    // Inicializo el generador de números aleatorios
    srand((unsigned int)time(NULL));

    while (1) {
        printf("Esperando conexiones...\n");
        if ((nuevo_socket = accept(servidor_fd, (struct sockaddr*)&direccion, &longitud_direccion)) == INVALID_SOCKET) {
            printf("Error. Codigo de error: %d\n", WSAGetLastError());
            closesocket(servidor_fd);
            WSACleanup();
            return 1;
        }

        while (1) {
            // Recibo los datos del cliente
            int longitud_recibida = recv(nuevo_socket, buffer, MAX_LONG, 0);
            if (longitud_recibida > 0) {
                buffer[longitud_recibida] = '\0';
                printf("Solicitud recibida: %s\n", buffer);

                // Hago que se procese la solicitud
                int longitud = atoi(buffer + 1);
                if (buffer[0] == 'U') {  // Generar nombre de usuario
                    if (longitud < 5 || longitud > 15) {
                        strcpy(respuesta, "Error: Longitud incorrecta para nombre de usuario.");
                    } else {
                        generar_nombre_usuario(respuesta, longitud);
                    }
                } else if (buffer[0] == 'P') {  // Generar contraseña
                    if (longitud < 8 || longitud >= 50) {
                        strcpy(respuesta, "Error: Longitud incorrecta para contrasenia.");
                    } else {
                        generar_contrasena(respuesta, longitud);
                    }
                } else {
                    strcpy(respuesta, "Error: Solicitud no valida.");
                }

                // Envio la respuesta al cliente
                if (send(nuevo_socket, respuesta, strlen(respuesta), 0) == SOCKET_ERROR) {
                    printf("Error al enviar datos. Codigo de error: %d\n", WSAGetLastError());
                } else {
                    printf("Respuesta enviada: %s\n", respuesta);
                }
            } else if (longitud_recibida == 0) {
                // El cliente cierra la conexion
                printf("El cliente ha cerrado la conexion.\n");
                break;
            } else {
                // Error en la recepcion de datos
                printf("Error al recibir datos. Codigo de error: %d\n", WSAGetLastError());
                break;
            }
        }

        // cierro el socket del cliente
        closesocket(nuevo_socket);
    }

    closesocket(servidor_fd);
    WSACleanup();
    return 0;
}
