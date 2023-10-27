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
        exit_with_error("%s\n", "malloc error");
    return ptr;
}

void* Calloc(size_t nitems, size_t size) {
    void* ptr = calloc(nitems, size);
    if(ptr == NULL) 
        exit_with_error("%s\n", "calloc error");
    return ptr;
}

void* Realloc(void* ptr, size_t size) {
    if(size == 0) {
        Free(ptr);
        return NULL;
    }

    ptr = realloc(ptr, size);
    if(ptr == NULL) 
        exit_with_error("%s\n", "realloc error");
    return ptr;
}

void Free(void* ptr) {
    free(ptr);
    ptr = NULL;
}


FILE* Fopen(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);
    if(fp == NULL) 
        exit_with_error("%s\n", "fopen error");
    return fp;
}

void Fclose(FILE* stream) {
    int err = fclose(stream);
    if(err != 0) 
        exit_with_error("%s\n", "fclose error");
}


void _WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData) {
    int res = WSAStartup(wVersionRequired, lpWSAData);
    if (res != 0)
        exit_with_error("WSAStartup error: %d\n", WSAGetLastError());
}


SOCKET Socket(int af, int type, int protocol) {
    SOCKET s = socket(af, type, protocol); 
    if (s == INVALID_SOCKET)
        exit_with_error("socket error: %lu\n", WSAGetLastError());
    return s;
}

void Bind(SOCKET s, const struct sockaddr *addr, int namelen) {
    int res = bind(s, addr, namelen);
    if (res == SOCKET_ERROR)
        exit_with_error("bind error: %lu\n", WSAGetLastError());
}

void Listen(SOCKET s, int backlog) {
    int res = listen(s, backlog);
    if (res == SOCKET_ERROR)
        exit_with_error("listen error: %lu\n", WSAGetLastError());
}

SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen) {
    SOCKET client_socket = accept(s, addr, addrlen);
    if (client_socket == INVALID_SOCKET)
        exit_with_error("accept error: %lu\n", WSAGetLastError());
    return client_socket;
}

void Connect(SOCKET s, const struct sockaddr *name, int namelen) {
    int res = connect(s, name, namelen);
    if (res == SOCKET_ERROR)
        exit_with_error("connect error: %lu\n", WSAGetLastError());
}


int Send(SOCKET s, const char *buf, int len, int flags) {
    int numBytes = send(s, buf, len, flags);
    if (numBytes == SOCKET_ERROR)
        exit_with_error("send error: %lu\n", WSAGetLastError());
    return numBytes;
}

int Recv(SOCKET s, char *buf, int len, int flags) {
    int numBytes = recv(s, buf, len, flags);
    if (numBytes == SOCKET_ERROR)
        exit_with_error("recv error: %lu\n", WSAGetLastError());
    return numBytes;
}


void Getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult) {
    int res = getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
    if (res != 0)
        exit_with_error("getaddrinfo error: %d\n", res);
}


HANDLE _CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE hFile = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    if(hFile == INVALID_HANDLE_VALUE)
        exit_with_error("CreateFileW error: %lu\n", GetLastError());
    return hFile;
}


HANDLE _CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE  lpStartAddress, __drv_aliasesMem LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
    HANDLE hThread = CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    if(hThread == NULL)
        exit_with_error("CreateThread error: %lu\n", GetLastError());
    return hThread;
}

DWORD _WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    DWORD res = WaitForSingleObject(hHandle, dwMilliseconds);
    if(res == WAIT_FAILED)
        exit_with_error("WaitForSingleObject error: %lu\n", GetLastError());
    return res;
}

void _GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait) {
    if(!GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait))
        exit_with_error("GetOverlappedResult error: %lu\n", GetLastError());
}


void _ReadDirectoryChangesW( HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
    if(!ReadDirectoryChangesW(hDirectory, lpBuffer, nBufferLength, bWatchSubtree, dwNotifyFilter, lpBytesReturned, lpOverlapped, lpCompletionRoutine))
        exit_with_error("ReadDirectoryChangesW error: %lu\n", GetLastError());
}
