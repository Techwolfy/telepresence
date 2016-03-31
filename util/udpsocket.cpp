//UDPSocket.cpp

//Preprocessor Macros
//Simple switch for platform-specific code
#ifndef _WIN32	//Linux/POSIX support
	#define IFPOSIX(x) x
	#define IFWIN32(x)
	#define PERROR(x) perror(x)
	#define HERROR(x) herror(x)
#else			//Windows support
	#define IFPOSIX(x)
	#define IFWIN32(x) x
	#define PERROR(x) printf(x ": %d\n", WSAGetLastError())
	#define HERROR(x) PERROR(x)
#endif

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
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#include <stdexcept>
#include "util/udpsocket.h"

//Constructor
UDPSocket::UDPSocket() : open(false),
						 serverFD(0),
						 readFlags(IFPOSIX(MSG_TRUNC) IFWIN32(0)),
						 server{0},
						 client{0} {
	#ifdef _WIN32
		//Initialize WinSock
		WSADATA wsaData = {0};
		if(WSAStartup(0x0202, &wsaData) != 0) {
			printf("Error inializing WinSock2: %d\n", WSAGetLastError());
			throw std::runtime_error("winsock2 initialization failed");
		}
	#endif
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
		PERROR("Error binding socket");
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
			HERROR("Error finding local host");
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
			HERROR("Error finding remote host");
			return -1;
		}
		remoteAddressNum = *(unsigned long *)remoteHost->h_addr_list[0];
	}

	if(remotePort != NULL) {
		remotePortNum = htons(atoi(remotePort));
	}

	printf("Local host %s:%s found as %" PRIu32 ":%" PRIu16 ".\n", localAddress, localPort, IFWIN32((uint32_t))ntohl(localAddressNum), ntohs(localPortNum));
	printf("Remote host %s:%s found as %" PRIu32 ":%" PRIu16 ".\n", remoteAddress, remotePort, IFWIN32((uint32_t))ntohl(remoteAddressNum), ntohs(remotePortNum));
    return openSocket(localAddressNum, localPortNum, remoteAddressNum, remotePortNum);
}

//Close socket
void UDPSocket::closeSocket() {
	if(serverFD != 0) {
#ifndef _WIN32
		close(serverFD);
#else
		closesocket(serverFD);
		WSACleanup();
#endif
		open = false;
	}
}

//Tell the caller whether or not the socket was successfully opened
bool UDPSocket::isOpen() {
	return open;
}

//Set whether or not to block when reading or writing data (default: block)
int UDPSocket::blockRead(bool block) {
	//Socket must be open
	if(!open) {
		return -1;
	}
#ifndef _WIN32
	//Get current socket flags
	int socketFlags = fcntl(serverFD, F_GETFL, 0);
	if(socketFlags < 0) {
		PERROR("Error changing UDP socket blocking type");
		return -1;
	}

	//Add or remove O_NONBLOCK flag and save changes
	socketFlags |= block ? ~O_NONBLOCK : O_NONBLOCK;
	if(fcntl(serverFD, F_SETFL, socketFlags) < 0) {
		PERROR("Error changing UDP socket blocking type");
		return -1;
	}
#else
	//Update socket nonblocking I/O flag
	unsigned long blockMode = !block;	//0 disables nonblocking mode, nonzero values enable it
	if(ioctlsocket(serverFD, FIONBIO, &blockMode) < 0) {
		PERROR("Error changing UDP socket blocking type");
		return -1;
	}
#endif
	return 0;
}

//Set maximum length of socket recieve buffer
int UDPSocket::setRecieveBufferLength(int length) {
	//Socket must be open
	if(!open) {
		return -1;
	}
	//Update socket receive buffer length
	int result = setsockopt(serverFD, SOL_SOCKET, SO_RCVBUF, IFWIN32((const char *))&length, sizeof(length));
	if(result < 0) {
		PERROR("Error setting UDP socket recieve buffer length");
	}
	return result;
}

//Read data from bound socket
int UDPSocket::readData(void *data, int length) {
	return readData(data, length, NULL);
}

//Read data from bound socket and retrieve remote sender address
int UDPSocket::readData(void *data, int length, sockaddr_in *remote) {
	//Socket must be open
	if(!open) {
		return -1;
	}

	socklen_t remoteLength;
	if(remote != NULL) {
		remoteLength = sizeof(*remote);
	} else {
		remoteLength = 0;
	}

	//Read data from socket
	int readLength = recvfrom(serverFD, IFWIN32((char *))data, length, readFlags, (struct sockaddr *)remote, &remoteLength);
	if(readLength < 0) {
		if(IFPOSIX(errno != EAGAIN) IFWIN32(WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAEMSGSIZE)) {
			PERROR("Error reading from socket");
		}
		return -1;
	}
	if(readLength > length IFWIN32(|| WSAGetLastError() == WSAEMSGSIZE)) {
		printf("Received datagram larger than buffer!\n");
		printf("Expected bytes: %d Received bytes: %d\n", length, readLength);
		return -1;
	}

	return readLength;
}

//Write to client defined when socket opened
int UDPSocket::writeData(void *data, int length) {
	if(client.sin_addr.s_addr != 0) {
		return writeData(&client, data, length);
	} else {
		printf("Error writing to socket!\n");
		return -1;
	}
}

//Write to arbitrary client
int UDPSocket::writeData(sockaddr_in *remote, void *data, int length) {
	//Socket must be open
	if(!open) {
		return -1;
	}

	//Write data to socket
	if(sendto(serverFD, IFWIN32((const char *))data, length, 0, (struct sockaddr *)remote, sizeof(*remote)) < 0) {
		PERROR("Error writing to socket");
		return -1;
	}
	return 0;
}
