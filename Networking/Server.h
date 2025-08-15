#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>   // Must be first
#include <Ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

struct Server {
    int domain;
    int service;
    int protocol;
    u_long iface;
    int port;
    int backlog;

    struct sockaddr_in address;
    SOCKET socket;  // Use SOCKET type for Windows

    void (*launch)(struct Server *);
};

struct Server server_constructor(
    int domain,
    int service,
    int protocol,
    u_long iface,
    int port,
    int backlog,
    void (*launch)(struct Server *server)
);

#endif
