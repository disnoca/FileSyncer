#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "directory_event_listener.h"
#include "data_structures/hash_map/hash_map.h"
#include "wrapper_functions.h"

#define SERVER_CONFIG_FILE_NAME "server.conf"
#define DIRECTORY_LINKS_FILE_NAME "directorylinks.conf"

#define PORT_MAX_LENGTH 5


/**
 * Initializes the server and returns its socket.
 * 
 * @return the server's socket
*/
SOCKET InitServer() {
	char port[PORT_MAX_LENGTH + 1];

	FILE* fp = Fopen(SERVER_CONFIG_FILE_NAME, "r");
	fscanf(fp, "%*s\n%s", port);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	struct addrinfo *result = NULL;
    struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

	Getaddrinfo(NULL, port, &hints, &result);

	SOCKET sServer = Socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	Bind(sServer, result->ai_addr, (int) result->ai_addrlen);

	freeaddrinfo(result);

	return sServer;
}

int main(void) {
	SOCKET sServer = InitServer();

	HashMap* directoryLinks = CreateHashMap();
	FILE *fp = Fopen(DIRECTORY_LINKS_FILE_NAME, "r");

	WCHAR server_dir[PATH_MAX];
	WCHAR client_dir[PATH_MAX];

	while(fwscanf(fp, L"%ls %ls", server_dir, client_dir) == 2) {
		WCHAR* heap_server_dir = Malloc((wcslen(server_dir) + 1) * sizeof(WCHAR));
    	WCHAR* heap_client_dir = Malloc((wcslen(client_dir) + 1) * sizeof(WCHAR));
    	wcscpy(heap_server_dir, server_dir);
   		wcscpy(heap_client_dir, client_dir);

		HMPut(directoryLinks, heap_server_dir, heap_client_dir);
		StartDirectoryEventListener(heap_server_dir, testHandlerFunction);
	}

	return 0;
}
