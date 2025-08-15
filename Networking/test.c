#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Ws2tcpip.h>

#include "Server.h"

void launch(struct Server *server) {
    char buffer[30000];

    while (1) {
        printf("==== WAITING FOR CONNECTION ===\n");

        int address_length = sizeof(server->address);
        SOCKET new_socket = accept(server->socket,
                                   (struct sockaddr *)&server->address,
                                   &address_length);

        if (new_socket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        int valread = recv(new_socket, buffer, sizeof(buffer), 0);
        if (valread > 0) {
            printf("%.*s\n", valread, buffer);

            FILE *file = fopen("index.html", "rb");
            if (!file) {
                const char *error = 
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "File not found";
                send(new_socket, error, strlen(error), 0);
            } else {
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                rewind(file);

                char *file_content = malloc(file_size);
                fread(file_content, 1, file_size, file);
                fclose(file);

                char header[256];
                sprintf(header,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %ld\r\n\r\n",
                        file_size);

                send(new_socket, header, strlen(header), 0);
                send(new_socket, file_content, file_size, 0);

                free(file_content);
            }
        }

        closesocket(new_socket);
    }
}


int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    struct Server server = server_constructor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8080, 10, launch);
    server.launch(&server);

    WSACleanup();
}
