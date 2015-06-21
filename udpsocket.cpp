//UDPSocket.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "udpsocket.h"

//Variables
const char *UDPSocket::SOCKET_DEFAULT_SERVER_ADDRESS = "0.0.0.0";
const char *UDPSocket::SOCKET_DEFAULT_CLIENT_ADDRESS = "127.0.0.1";
const char *UDPSocket::SOCKET_DEFAULT_PORT = "8353";

//Constructor
UDPSocket::UDPSocket() : serverFD(0),
						 server{0},
						 client{0} {

}

//Destructor
UDPSocket::~UDPSocket() {
	closeSocket();
}

//Functions
//Open and bind a UDP socket
int UDPSocket::openSocket(unsigned long address, int port) {
	//Make sure an existing socket doesn't get leaked
	if(serverFD != 0) {
		closeSocket();
	}

	//Open socket
	serverFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(serverFD < 0) {
		printf("Error opening socket!\n");
		return -1;
	}

	//Set up server struct
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);		//0.0.0.0
	server.sin_port = port;

	//Set up client struct
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = address;
	client.sin_port = port;

	//Bind socket
	if(bind(serverFD, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Error binding socket!\n");
		return -1;
	}

	return 0;
}

//Map provided address and port to network values
int UDPSocket::openSocket(const char *address, const char *port) {
	unsigned long addressNum = 0;
	int portNum = htons(atoi(port));

	//Look up the hostname stored in address
	struct hostent *host = gethostbyname(address);
	if(host == NULL) {
		printf("Error finding host!\n");
		return -1;
	}
	
	addressNum = *(unsigned long *)host->h_addr_list[0];
	printf("Host %s:%s found as %lu:%d.\n", address, port, addressNum, portNum);
    return openSocket(addressNum, portNum);
}

//Close socket
void UDPSocket::closeSocket() {
	if(serverFD != 0) {
		close(serverFD);
	}
}

//Read data from bound socket
sockaddr_in* UDPSocket::readData(void *data, int length) {
	socklen_t remoteLength = sizeof(remote);
	if(recvfrom(serverFD, data, length, 0, (struct sockaddr *)&remote, &remoteLength) <= 0) {
		printf("Error reading from socket!\n");
		return NULL;
	}

	return &remote;
}

//Write to client defined when socket opened
void UDPSocket::writeData(void *data, int length) {
	if(sendto(serverFD, data, length, 0, (struct sockaddr *)&client, sizeof(client)) < 0) {
		printf("Error writing to socket!\n");
	}
}

//Write to arbitrary client
void UDPSocket::writeData(sockaddr_in *remote, void *data, int length) {
	if(sendto(serverFD, data, length, 0, (struct sockaddr *)remote, sizeof(&remote)) < 0) {
		printf("Error writing to socket!\n");
	}
}