#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "wrapper_functions.h"

void exit_with_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

void* Malloc(size_t size) {
    void* ptr = malloc(size);
    if(ptr == NULL) 
        exit_with_error("%s\n", "Malloc error");
    return ptr;
}

void* Calloc(size_t nitems, size_t size) {
    void* ptr = calloc(nitems, size);
    if(ptr == NULL) 
        exit_with_error("%s\n", "Calloc error");
    return ptr;
}

void* Realloc(void* ptr, size_t size) {
    if(size == 0) {
        Free(ptr);
        return NULL;
    }

    ptr = realloc(ptr, size);
    if(ptr == NULL) 
        exit_with_error("%s\n", "Realloc error");
    return ptr;
}

void Free(void* ptr) {
    free(ptr);
    ptr = NULL;
}


void _WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData) {
    int err = WSAStartup(wVersionRequired, lpWSAData);
    if (err != 0)
        exit_with_error("WSAStartup error: %d\n", err);
}


SOCKET Socket(int af, int type, int protocol) {
    SOCKET s = socket(af, type, protocol); 
    if (s == -1)
        exit_with_error("Socket error: %lu\n", GetLastError());
    return s;
}

void Bind(SOCKET s, const struct sockaddr *addr, int namelen) {
    int err = bind(s, addr, namelen);
    if (err == -1)
        exit_with_error("Bind error: %lu\n", err);
}

void Listen(SOCKET s, int backlog) {
    int err = listen(s, backlog);
    if (err == -1)
        exit_with_error("Listen error: %lu\n", err);
}

SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen) {
    SOCKET client_socket = accept(s, addr, addrlen);
    if (client_socket == INVALID_SOCKET)
        exit_with_error("Accept error: %lu\n", GetLastError());
    return client_socket;
}

void Connect(SOCKET s, const struct sockaddr *name, int namelen) {
    int err = connect(s, name, namelen);
    if (err == -1)
        exit_with_error("Connect error: %lu\n", err);
}


int Send(SOCKET s, const char *buf, int len, int flags) {
    int numBytes = send(s, buf, len, flags);
    if (numBytes == -1)
        exit_with_error("Send error: %lu\n", numBytes);
    return numBytes;
}

int Recv(SOCKET s, char *buf, int len, int flags) {
    int numBytes = recv(s, buf, len, flags);
    if (numBytes == -1)
        exit_with_error("Recv error: %lu\n", GetLastError());
    return numBytes;
}


void Getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult) {
    int err = getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
    if (err != 0)
        exit_with_error("Getaddrinfo error: %d\n", err);
}
