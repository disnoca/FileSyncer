#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "wrapper_functions.h"


/**
 * Connects to the server and returns the socket.
 * 
 * @return the server's socket
*/
SOCKET connect_to_server(char* ip_addr, char* port) {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

	Getaddrinfo(ip_addr, port, &hints, &result);

	SOCKET sConn = Socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	Connect(sConn, result->ai_addr, (int) result->ai_addrlen);

	freeaddrinfo(result);

	return sConn;
}


int main(int argc, char** argv) {
	SOCKET sConn = connect_to_server(argv[1], argv[2]);

	char* buffer = Calloc(1024, sizeof(char));
	sprintf(buffer, "Hello from client!\n");

	Send(sConn, buffer, 1024, 0);

	closesocket(sConn);
    WSACleanup();

	return 0;
}
