//UDPSocket.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#ifndef _WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
#else
	#include <winsock.h>
#endif
#include <netinet/in.h>
#include "util/udpsocket.h"

//Constructor
UDPSocket::UDPSocket() : open(false),
						 serverFD(0),
						 readFlags(MSG_TRUNC),
						 server{0},
						 client{0} {

}

//Destructor
UDPSocket::~UDPSocket() {
	closeSocket();
}

//Functions
//Open and bind a UDP socket
int UDPSocket::openSocket(unsigned long localAddress, int localPort, unsigned long remoteAddress, int remotePort) {
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
	server.sin_addr.s_addr = localAddress;
	server.sin_port = localPort;

	//Set up client struct
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = remoteAddress;
	client.sin_port = remotePort;

	//Bind socket
	if(bind(serverFD, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Error binding socket!\n");
		return -1;
	}

	open = true;
	return 0;
}

//Map provided address and port to network values
int UDPSocket::openSocket(const char *localAddress, const char *localPort, const char *remoteAddress, const char *remotePort) {
	unsigned long localAddressNum = 0;
	unsigned long remoteAddressNum = 0;
	int localPortNum = 0;
	int remotePortNum = 0;

	//Look up the hostname stored in localAddress, if any
	if(localAddress == NULL) {
		localAddressNum = htonl(INADDR_ANY);	//0.0.0.0
	} else {
		struct hostent *localHost = gethostbyname(localAddress);
		if(localHost == NULL) {
			printf("Error finding local host!\n");
			return -1;
		} else {
			localAddressNum = *(unsigned long *)localHost->h_addr_list[0];
		}
	}

	if(localPort != NULL) {
		localPortNum = htons(atoi(localPort));
	}

	//Look up the hostname stored in remoteAddress
	if(remoteAddress != NULL) {
		struct hostent *remoteHost = gethostbyname(remoteAddress);
		if(remoteHost == NULL) {
			printf("Error finding remote host!\n");
			return -1;
		}
		remoteAddressNum = *(unsigned long *)remoteHost->h_addr_list[0];
	}

	if(remotePort != NULL) {
		remotePortNum = htons(atoi(remotePort));
	}

	printf("Local host %s:%s found as %" PRIu32 ":%" PRIu16 ".\n", localAddress, localPort, ntohl(localAddressNum), ntohs(localPortNum));
	printf("Remote host %s:%s found as %" PRIu32 ":%" PRIu16 ".\n", remoteAddress, remotePort, ntohl(remoteAddressNum), ntohs(remotePortNum));
    return openSocket(localAddressNum, localPortNum, remoteAddressNum, remotePortNum);
}

//Close socket
void UDPSocket::closeSocket() {
	if(serverFD != 0) {
		close(serverFD);
		open = false;
	}
}

//Tell the caller whether or not the socket was successfully opened
bool UDPSocket::isOpen() {
	return open;
}

//Set whether or not to block when reading or writing data (default: block)
void UDPSocket::blockRead(bool block) {
	if(block) {
		readFlags &= ~MSG_DONTWAIT;
	} else {
		readFlags |= MSG_DONTWAIT;
	}
}

//Set maximum length of socket recieve buffer
int UDPSocket::setRecieveBufferLength(int length) {
	int result = setsockopt(serverFD, SOL_SOCKET, SO_RCVBUF, &length, sizeof(length));
	if(result < 0) {
		perror("Error setting UDP socket recieve buffer length");
	}
	return result;
}

//Read data from bound socket
int UDPSocket::readData(void *data, int length) {
	return readData(data, length, NULL);
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
	} else if(readLength > length) {
		printf("Received datagram larger than buffer!\n");
		printf("Expected bytes: %d Received bytes: %d\n", length, readLength);
		return -1;
	}

	return readLength;
}

//Write to client defined when socket opened
void UDPSocket::writeData(void *data, int length) {
	if(client.sin_addr.s_addr != 0) {
		writeData(&client, data, length);
	} else {
		printf("Error writing to socket!\n");
	}
}

//Write to arbitrary client
void UDPSocket::writeData(sockaddr_in *remote, void *data, int length) {
	if(sendto(serverFD, data, length, 0, (struct sockaddr *)remote, sizeof(*remote)) < 0) {
		printf("Error writing to socket!\n");
	}
}
