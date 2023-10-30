#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "data_structures/hash_map/hash_map.h"
#include "directory_event_listener.h"
#include "wrapper_functions.h"

#define SERVER_CONFIG_FILE_NAME "server.conf"
#define DIRECTORY_LINKS_FILE_NAME "directory_links.conf"

#define HOSTNAME_MAX_LENGTH 	255
#define PORT_MAX_LENGTH 		5

/**
 * Connects to the server and returns the socket.
 * 
 * @return the server's socket
*/
SOCKET ConnectToServer() {
	char hostname[HOSTNAME_MAX_LENGTH + 1];
	char port[PORT_MAX_LENGTH + 1];

	FILE* fp = Fopen(SERVER_CONFIG_FILE_NAME, "r");
	fscanf(fp, "%s\n%s", hostname, port);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	Getaddrinfo(hostname, port, &hints, &result);

	SOCKET sConn = Socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// Attempt to connect to an address until one succeeds
    for(struct addrinfo *ptr = result; ptr != NULL ; ptr = ptr->ai_next) {
        sConn = Socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if(connect(sConn, ptr->ai_addr, (int)ptr->ai_addrlen) != SOCKET_ERROR)
			break;
        
		closesocket(sConn);
        sConn = INVALID_SOCKET;
    }

	if(sConn == INVALID_SOCKET)
		ExitWithError("Unable to connect to server\n");

	freeaddrinfo(result);
	return sConn;
}


int main(void) {
	SOCKET sConn = ConnectToServer();

	HashMap* directoryLinks = HMcreate();
	FILE *fp = Fopen(DIRECTORY_LINKS_FILE_NAME, "r");

	WCHAR serverDir[PATH_MAX];
	WCHAR clientDir[PATH_MAX];

	while(fwscanf(fp, L"%ls %ls", serverDir, clientDir) == 2) {
		WCHAR* heapServerDir = Malloc((wcslen(serverDir) + 1) * sizeof(WCHAR));
    	WCHAR* heapClientDir = Malloc((wcslen(clientDir) + 1) * sizeof(WCHAR));
    	wcscpy(heapServerDir, serverDir);
   		wcscpy(heapClientDir, clientDir);

		HMput(directoryLinks, heapClientDir, heapServerDir);
		// call function to check for discrepancies beteween server and client directories
		StartDirectoryEventListener(heapClientDir, NULL);
	}

	return 0;
}
