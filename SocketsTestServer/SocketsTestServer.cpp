// SocketsTestServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdlib.h"
#include "string.h"
#include "winsock2.h"						//
#include "ws2tcpip.h"
#include "iostream"

#define PORT 7890			//The port we shall open our socket on.

void fatal(char *message) //A function to display an error message and then exit.
{

	char error_message[100];	//A char string of 100 bytes

	strcpy_s(error_message, "[!!] Fatal Error ");	//Copying the 2nd arg to error_message, use _s to make the function safer to avoid potential buffer overflows.
	strncat_s(error_message, message, 83);	//Appends the a number of characters (defined by arg 3) form the src char (arg 2) to the dest char (arg 1). In this case, 81, because error_message is 100 bytes, and it already contains 17 bytes from the strcpy.
	perror(error_message);
	exit(-1);

}


void dump(char *data_buffer, const unsigned int length)
{

	unsigned char byte;
	unsigned int i, j;

	for (i = 0; i < length; i++)
	{

		byte = data_buffer[i];
		printf("%02x ", data_buffer[i]);

		if (((i % 16) == 15) || (i == length - 1))
		{
			for (j = 0; j < 15 - (i % 16); j++)
				printf("   ");
			printf("| ");
			for (j = (i - (i % 16)); j <= i; j++)
			{
				byte = data_buffer[j];
				if ((byte > 31) && (byte < 127))
					printf("%c", byte);
				else
					printf(".");
			}
			printf("\n");
		}	//End if

	}	//End for

}


int main(void)
{
    
	//These next five lines aren't in Erickson's original code because his was written in C, and used different libraries for sockets e.g. sockets.h. As this is C++ I'm using winsock, and WSAStartup() msut be called before any other winsock function can be used.

	WORD wVersionRequested;		//16 byte data unit to contain winsock's version.
	WSADATA wsaData;			//WSDATA is a data structure containing a bunch of members containing information about the windows socket implementation, e.g. version, description, server status.
	
	wVersionRequested = MAKEWORD(2, 2);		//MAKEWORD links together the two bytes 2 and 2; we are looking for winsock version 2.2.

	if (WSAStartup(wVersionRequested, &wsaData) != 0)		//WSAStartup returns details of winsocks implementation to the data structure pointed to by &wsaData, the function returns 0 if successful.
		fatal(" with WSAStartup");



	int sockfd, new_sockfd;								//sockfd will be 
	struct sockaddr_in host_addr, client_addr;
	socklen_t sin_size;
	int recv_length = 1, yes = 1;
	char buffer[1024];

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		fatal("in socket");
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int)) == INVALID_SOCKET)
		fatal("setting socket option SO_REUSEADDR");

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = 0;
	memset(&(host_addr.sin_zero), '\0', 8);

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == INVALID_SOCKET)
		fatal("binding to socket");

	if (listen(sockfd, 5) == INVALID_SOCKET)
		fatal("listening on socket");

	while (1)		//Accept loops, while(1) will run infinitely until a break statement is called.
	{

		sin_size = sizeof(struct sockaddr_in);\
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);

		if (new_sockfd == INVALID_SOCKET)
			fatal("accepting connection");

		printf("server: got connection from %s port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		send(new_sockfd, "Hello, world!\n", 13, 0);
		recv_length = recv(new_sockfd, buffer, 1024, 0);

		while (recv_length > 0)
		{

			printf("RECV: %d bytes\n", recv_length);
			dump(buffer, recv_length);
			recv_length = recv(new_sockfd, buffer, 1024, 0);

		}

		closesocket(new_sockfd);

	}

	return 0;
}

