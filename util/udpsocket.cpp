//UDPSocket.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "util/udpsocket.h"

//Variables
const char *UDPSocket::SOCKET_DEFAULT_SERVER_ADDRESS = "0.0.0.0";
const char *UDPSocket::SOCKET_DEFAULT_CLIENT_ADDRESS = "127.0.0.1";
const char *UDPSocket::SOCKET_DEFAULT_PORT = "8353";

//Constructor
UDPSocket::UDPSocket() : serverFD(0),
						 readFlags(0),
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
	printf("Host %s:%s found as %lu:%d.\n", address, port, ntohl(addressNum), ntohs(portNum));
    return openSocket(addressNum, portNum);
}

//Close socket
void UDPSocket::closeSocket() {
	if(serverFD != 0) {
		close(serverFD);
	}
}

//Set whether or not to block when reading or writing data (default: block)
void UDPSocket::blockRead(bool block) {
	if(block) {
		readFlags &= ~MSG_DONTWAIT;
	} else {
		readFlags |= MSG_DONTWAIT;
	}
}

//Read data from bound socket
int UDPSocket::readData(void *data, int length) {
	readData(data, length, NULL);
}

//Read data from bound socket and retrieve remote sender address
int UDPSocket::readData(void *data, int length, sockaddr_in *remote) {
	socklen_t remoteLength;
	if(remote != NULL) {
		remoteLength = sizeof(*remote);
	} else {
		remoteLength = 0;
	}

	int readLength = recvfrom(serverFD, data, length, readFlags, (struct sockaddr *)remote, &remoteLength);
	if(readLength < 0) {
		if(errno != EAGAIN) {
			printf("Error reading from socket!\n");
		}
		return -1;
	} else if(readLength != length) {
		printf("Not enough bytes read from socket!\n");
		printf("Expected bytes: %d Received bytes: %d\n", length, readLength);
		return -1;
	}

	if(remote != NULL) {
		printf("Packet received from %lu:%d.\n", ntohl(remote->sin_addr.s_addr), ntohs(remote->sin_port));
	}
	return readLength;
}

//Write to client defined when socket opened
void UDPSocket::writeData(void *data, int length) {
	writeData(&client, data, length);
}

//Write to arbitrary client
void UDPSocket::writeData(sockaddr_in *remote, void *data, int length) {
	if(sendto(serverFD, data, length, 0, (struct sockaddr *)remote, sizeof(*remote)) < 0) {
		printf("Error writing to socket!\n");
	} else {
		printf("Packet sent to %lu:%d.\n", ntohl(remote->sin_addr.s_addr), ntohs(remote->sin_port));
	}
}
