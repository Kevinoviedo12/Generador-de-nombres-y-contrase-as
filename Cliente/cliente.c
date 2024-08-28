#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PUERTO 9090
#define MAX_LONG 1024

int main() {
    WSADATA wsa;
    SOCKET socket_cliente;
    struct sockaddr_in direccion_servidor;
    char buffer[MAX_LONG] = {0};
    char respuesta[MAX_LONG] = {0};
    int opcion;
    int longitud;

    // Inicializar Winsock
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en iniciar Winsock. Codigo de error: %d\n", WSAGetLastError());
        return 1;
    }

    // Creo el socket
    if ((socket_cliente = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("No se pudo crear el socket. Codigo de error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_port = htons(PUERTO);
    direccion_servidor.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Coencto al servidor
    if (connect(socket_cliente, (struct sockaddr*)&direccion_servidor, sizeof(direccion_servidor)) == SOCKET_ERROR) {
        printf("Error al conectar con el servidor. Codigo de error: %d\n", WSAGetLastError());
        closesocket(socket_cliente);
        WSACleanup();
        return 1;
    }

    while (1) {
        printf("Menu:\n");
        printf("1. Generar nombre de usuario\n");
        printf("2. Generar contrasenia\n");
        printf("3. Salir\n");
        printf("Elija una opcion: ");
        scanf("%d", &opcion);

        if (opcion == 3) {
            printf("Saliendo del programa...\n");
            break; // Salir del bucle y cerrar el cliente
        }

        if (opcion == 1 || opcion == 2) {
            while (1) {
                printf("Indique la longitud: ");
                scanf("%d", &longitud);

                if (opcion == 1) {
                    if (longitud < 5 || longitud > 15) {
                        printf("Longitud incorrecta para nombre de usuario. Tiene que estar entre 5 y 15.\n");
                    } else {
                        snprintf(buffer, MAX_LONG, "U%d", longitud);
                        break; // Longitud válida, salir del bucle interno
                    }
                } else if (opcion == 2) {
                    if (longitud < 8 || longitud >= 50) {
                        printf("Longitud incorrecta para contrasenia. Tiene que ser al menos 8 y menos de 50.\n");
                    } else {
                        snprintf(buffer, MAX_LONG, "P%d", longitud);
                        break; // Longitud válida, salir del bucle interno
                    }
                }
            }

            // Enviar la solicitud al servidor
            if (send(socket_cliente, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
                printf("Error al enviar solicitud. Codigo de error: %d\n", WSAGetLastError());
                break; // Salir del bucle en caso de error de envío
            }
            printf("Solicitud enviada: %s\n", buffer);

            // recibo respuesta del servidor
            int longitud_recibida = recv(socket_cliente, respuesta, MAX_LONG, 0);
            if (longitud_recibida > 0) {
                respuesta[longitud_recibida] = '\0'; // Asegurar el null-terminator
                printf("Respuesta del servidor: %s\n", respuesta);
            } else if (longitud_recibida == 0) {
                printf("El servidor ha cerrado la conexión.\n");
                break; // Salir del bucle si el servidor cierra la conexión
            } else {
                printf("Error al recibir datos. Código de error: %d\n", WSAGetLastError());
                break; // Salir en caso de error de recepción
            }
        } else {
            printf("Opción no válida.\n");
        }
    }

    // cierro el socket
    closesocket(socket_cliente);
    WSACleanup();
    return 0;
}
