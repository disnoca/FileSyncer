#ifndef _WRAPPER_FUNCTIONS_H
#define _WRAPPER_FUNCTIONS_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>

void exit_with_error(const char* format, ...);

void* Malloc(size_t size);
void* Calloc(size_t nitems, size_t size);
void* Realloc(void* ptr, size_t size);
void Free(void* ptr);

FILE* Fopen(const char* filename, const char* mode);
void Fclose(FILE* stream);

void _WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);

SOCKET Socket(int af, int type, int protocol);
void Bind(SOCKET s, const struct sockaddr *addr, int namelen);
void Listen(SOCKET s, int backlog);
SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen);
void Connect(SOCKET s, const struct sockaddr *name, int namelen);

int Send(SOCKET s, const char *buf, int len, int flags);
int Recv(SOCKET s, char *buf, int len, int flags);

void Getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);

#endif
