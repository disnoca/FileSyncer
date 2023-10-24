#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "wrapper_functions.h"


/**
 * Initializes the server and returns its socket.
 * 
 * @return the server's socket
*/
SOCKET init_server(char* port) {
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

int main(int argc, char** argv) {
	SOCKET sServer = init_server(argv[1]);

	Listen(sServer, SOMAXCONN);
	SOCKET sClient = Accept(sServer, NULL, NULL);

	char* buffer = Calloc(1024, sizeof(char));

	Recv(sClient, buffer, 1024, 0);
	printf("%s\n", buffer);

	closesocket(sClient);

	return 0;
}
