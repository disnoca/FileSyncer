#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "wrapper_functions.h"

void ExitWithError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

void* Malloc(size_t size) {
    void* ptr = malloc(size);
    if(ptr == NULL) 
        ExitWithError("%s\n", "malloc error");
    return ptr;
}

void* Calloc(size_t nitems, size_t size) {
    void* ptr = calloc(nitems, size);
    if(ptr == NULL) 
        ExitWithError("%s\n", "calloc error");
    return ptr;
}

void* Realloc(void* ptr, size_t size) {
    if(size == 0) {
        Free(ptr);
        return NULL;
    }

    ptr = realloc(ptr, size);
    if(ptr == NULL) 
        ExitWithError("%s\n", "realloc error");
    return ptr;
}

void Free(void* ptr) {
    free(ptr);
    ptr = NULL;
}


FILE* Fopen(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);
    if(fp == NULL) 
        ExitWithError("%s\n", "fopen error");
    return fp;
}

void Fclose(FILE* stream) {
    int err = fclose(stream);
    if(err != 0) 
        ExitWithError("%s\n", "fclose error");
}


void _WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData) {
    int res = WSAStartup(wVersionRequired, lpWSAData);
    if (res != 0)
        ExitWithError("WSAStartup error: %d\n", WSAGetLastError());
}


SOCKET Socket(int af, int type, int protocol) {
    SOCKET s = socket(af, type, protocol); 
    if (s == INVALID_SOCKET)
        ExitWithError("socket error: %lu\n", WSAGetLastError());
    return s;
}

void Bind(SOCKET s, const struct sockaddr *addr, int namelen) {
    int res = bind(s, addr, namelen);
    if (res == SOCKET_ERROR)
        ExitWithError("bind error: %lu\n", WSAGetLastError());
}

void Listen(SOCKET s, int backlog) {
    int res = listen(s, backlog);
    if (res == SOCKET_ERROR)
        ExitWithError("listen error: %lu\n", WSAGetLastError());
}

SOCKET Accept(SOCKET s, struct sockaddr *addr, int *addrlen) {
    SOCKET client_socket = accept(s, addr, addrlen);
    if (client_socket == INVALID_SOCKET)
        ExitWithError("accept error: %lu\n", WSAGetLastError());
    return client_socket;
}

void Connect(SOCKET s, const struct sockaddr *name, int namelen) {
    int res = connect(s, name, namelen);
    if (res == SOCKET_ERROR)
        ExitWithError("connect error: %lu\n", WSAGetLastError());
}


int Send(SOCKET s, const char *buf, int len, int flags) {
    int numBytes = send(s, buf, len, flags);
    if (numBytes == SOCKET_ERROR)
        ExitWithError("send error: %lu\n", WSAGetLastError());
    return numBytes;
}

int Recv(SOCKET s, char *buf, int len, int flags) {
    int numBytes = recv(s, buf, len, flags);
    if (numBytes == SOCKET_ERROR)
        ExitWithError("recv error: %lu\n", WSAGetLastError());
    return numBytes;
}


void Getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult) {
    int res = getaddrinfo(pNodeName, pServiceName, pHints, ppResult);
    if (res != 0)
        ExitWithError("getaddrinfo error: %d\n", res);
}


HANDLE _CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    HANDLE hFile = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    if(hFile == INVALID_HANDLE_VALUE)
        ExitWithError("CreateFileW error: %lu\n", GetLastError());
    return hFile;
}


HANDLE _CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE  lpStartAddress, __drv_aliasesMem LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
    HANDLE hThread = CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
    if(hThread == NULL)
        ExitWithError("CreateThread error: %lu\n", GetLastError());
    return hThread;
}

DWORD _WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds) {
    DWORD res = WaitForSingleObject(hHandle, dwMilliseconds);
    if(res == WAIT_FAILED)
        ExitWithError("WaitForSingleObject error: %lu\n", GetLastError());
    else if(res == WAIT_ABANDONED)
        ExitWithError("WaitForSingleObject error: WAIT_ABANDONED\n");
    return res;
}

HANDLE _CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName) {
    HANDLE hEvent = CreateEventW(lpEventAttributes, bManualReset, bInitialState, lpName);
    if(hEvent == NULL)
        ExitWithError("CreateEventW error: %lu\n", GetLastError());
    return hEvent;
}

void _GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait) {
    if(!GetOverlappedResult(hFile, lpOverlapped, lpNumberOfBytesTransferred, bWait))
        ExitWithError("GetOverlappedResult error: %lu\n", GetLastError());
}

HANDLE _CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName) {
    HANDLE hMutex = CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
    if(hMutex == NULL)
        ExitWithError("CreateMutexW error: %lu\n", GetLastError());
    return hMutex;
}

void _ReleaseMutex(HANDLE hMutex) {
    if(!ReleaseMutex(hMutex))
        ExitWithError("ReleaseMutex error: %lu\n", GetLastError());
}

void _SetEvent(HANDLE hEvent) {
    if(!SetEvent(hEvent))
        ExitWithError("SetEvent error: %lu\n", GetLastError());
}

void _ResetEvent(HANDLE hEvent) {
    if(!ResetEvent(hEvent))
        ExitWithError("ResetEvent error: %lu\n", GetLastError());
}


void _ReadDirectoryChangesW( HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
    if(!ReadDirectoryChangesW(hDirectory, lpBuffer, nBufferLength, bWatchSubtree, dwNotifyFilter, lpBytesReturned, lpOverlapped, lpCompletionRoutine))
        ExitWithError("ReadDirectoryChangesW error: %lu\n", GetLastError());
}
