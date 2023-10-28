#ifndef WRAPPER_FUNCTIONS_H_INCLUDED
#define WRAPPER_FUNCTIONS_H_INCLUDED

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

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


HANDLE _CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

HANDLE _CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE  lpStartAddress, __drv_aliasesMem LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
DWORD _WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
HANDLE _CreateEventW(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCWSTR lpName);
void _GetOverlappedResult(HANDLE hFile, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
HANDLE _CreateMutexW(LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCWSTR lpName);
void _ReleaseMutex(HANDLE hMutex);

void _ReadDirectoryChangesW( HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

#endif
