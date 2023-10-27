#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "directory_event_listener.h"
#include "wrapper_functions.h"

#define SERVER_CONFIG_FILE_NAME "server.conf"

#define PORT_MAX_LENGTH 5

#define FILE_DATA_BATCH_MAX_SIZE 1048576


void send_file() {

}


/**
 * Initializes the server and returns its socket.
 * 
 * @return the server's socket
*/
SOCKET init_server() {
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
	SOCKET sServer = init_server();

	return 0;
}
