//UDPSocket.cpp

//Preprocessor Macros
//Simple switch for platform-specific code
#ifndef _WIN32	//Linux/POSIX support
	#define IFPOSIX(x) x
	#define IFWIN32(x)
	#define PERROR(x) perror(x)
	#define GAIERROR(x, y) printf(x ": %s\n", gai_strerror(y))
#else			//Windows support
	#define IFPOSIX(x)
	#define IFWIN32(x) x
	#define PERROR(x) printf(x ": %d\n", WSAGetLastError())
	#define GAIERROR(x, y) printf(x ": %d\n", WSAGetLastError())
#endif

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
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
						 server(NULL),
						 client(NULL) {
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
//Print the IPv4 or IPv6 address:port pair contained in a sockaddr struct into the provided buffer
char* UDPSocket::getAddrString(struct sockaddr *addr, socklen_t addrLength, char *buffer, int length) {
	char address[NI_MAXHOST] = {'0'};	//First element is '0', all others are '\0'
	char port[NI_MAXSERV] = {'0'};		//First element is '0', all others are '\0'

	//Set address and port based on input address family
	if(addr != NULL) {
		int gniResult = 0;
		if((gniResult = getnameinfo(addr, addrLength, address, sizeof(address), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
			GAIERROR("Error retreiving address string", gniResult);
		}
	}

	//Print address:port pair; NULL and unknown address families are displayed as "[0]:0"
	snprintf(buffer, length, "[%s]:%s", address, port);

	//Return a pointer to the provided buffer
	return buffer;
}

//Open and bind a UDP socket
int UDPSocket::openSocket(struct addrinfo *localAddr, struct addrinfo *remoteAddr) {
	//Make sure an existing socket doesn't get leaked
	if(serverFD != 0) {
		closeSocket();
	}

	//Save server and client structs
	server = localAddr;
	client = remoteAddr;

	//Open socket
	serverFD = socket(server->ai_family, SOCK_DGRAM, 0);
	if(serverFD < 0) {
		printf("Error opening socket!\n");
		return -1;
	}

	//Set the socket to dual-stack, if using IPv6 and not the default
	if(server->ai_family == AF_INET6) {
		int ipv6only = 0;	//Not IPv6 only; allow both IPv6 and IPv6-mapped-IPv4 connections
		if(setsockopt(serverFD, IPPROTO_IPV6, IPV6_V6ONLY, IFWIN32((char *))&ipv6only, sizeof(ipv6only)) < 0) {
			PERROR("Error setting UDP socket as dual-stack IPv4/IPv6");
			return -1;
		}
	}

	//Bind socket
	if(bind(serverFD, server->ai_addr, server->ai_addrlen) < 0) {
		PERROR("Error binding socket");
		return -1;
	}

	//Print string representation of bound socket
	struct sockaddr_storage boundAddress = {0};
	socklen_t boundAddressLength = sizeof(boundAddress);
	int gsnResult = 0;
	if((gsnResult = getsockname(serverFD, (struct sockaddr *)&boundAddress, &boundAddressLength)) < 0) {
		GAIERROR("Error finding bound address", gsnResult);
		return -1;
	} else {
		char buffer[NI_MAXHOST + NI_MAXSERV] = {0};	//Hostname, colon, service name (or port number), null character
		printf("Bound to %s.\n", getAddrString((struct sockaddr *)&boundAddress, boundAddressLength, buffer, sizeof(buffer)));
	}

	open = true;
	return 0;
}

//Map provided address and port to network values
int UDPSocket::openSocket(const char *localAddress, const char *localPort, const char *remoteAddress, const char *remotePort) {
	struct addrinfo *local = NULL;
	struct addrinfo *remote = NULL;
	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;						//IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM;						//UDP
	hints.ai_flags = AI_ALL | AI_V4MAPPED | AI_ADDRCONFIG;	//Only return useful addresses, based on type and connectivity
	int gaiResult = 0;


	//Look up the hostname stored in localAddress, if any
	if(localAddress == NULL) {
		hints.ai_flags |= AI_PASSIVE;	//Use INADDR_ANY or INADDR6_ANY
	}
	if(localPort == NULL) {
		localPort = "0";
	}
	if((gaiResult = getaddrinfo(localAddress, localPort, &hints, &local)) != 0) {
		printf("Error finding local host: %s\n", gai_strerror(gaiResult));
		return -1;
	}

	//Look up the hostname stored in remoteAddress
	if(remoteAddress != NULL && remotePort != NULL) {
		hints.ai_family = local->ai_family;	//Make sure the client connection uses the same address family as the server socket
		hints.ai_flags &= ~AI_PASSIVE;		//Use value of remoteAddress
		if((gaiResult = getaddrinfo(remoteAddress, remotePort, &hints, &remote)) != 0) {
			printf("Error finding local host: %s\n", gai_strerror(gaiResult));
			return -1;
		}
	}

	//Print string representation of addrinfo structs
	char buffer[NI_MAXHOST + NI_MAXSERV] = {0};
	printf("Local host %s:%s found as %s.\n", (localAddress == NULL ? "0" : localAddress), (localPort == NULL ? "0" : localPort), getAddrString(local->ai_addr, local->ai_addrlen, buffer, sizeof(buffer)));
	if(remote != NULL) {
		printf("Remote host %s:%s found as %s.\n", (remoteAddress == NULL ? "0" : remoteAddress), (remotePort == NULL ? "0" : remotePort), getAddrString(remote->ai_addr, remote->ai_addrlen, buffer, sizeof(buffer)));
	}

	//Open the socket
	return openSocket(local, remote);
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

	if(server != NULL) {
		freeaddrinfo(server);
		server = NULL;
	}
	if(client != NULL) {
		freeaddrinfo(client);
		client = NULL;
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
	int result = setsockopt(serverFD, SOL_SOCKET, SO_RCVBUF, IFWIN32((char *))&length, sizeof(length));
	if(result < 0) {
		PERROR("Error setting UDP socket recieve buffer length");
	}
	return result;
}

//Read data from bound socket
int UDPSocket::readData(void *data, int length) {
	return readData(data, length, (struct sockaddr *)NULL, NULL);	//Null cast disambiguates function call
}

//Read data from bound socket and retrieve remote sender address
int UDPSocket::readData(void *data, int length, struct sockaddr *remote, socklen_t *remoteLength) {
	//Socket must be open
	if(!open) {
		return -1;
	}

	//Read data from socket
	int readLength = recvfrom(serverFD, IFWIN32((char *))data, length, readFlags, remote, remoteLength);
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

int UDPSocket::readData(void *data, int length, struct sockaddr_storage *remote, socklen_t *remoteLength) {
	return readData(data, length, (struct sockaddr *)remote, remoteLength);
}

//Write to client defined when socket opened
int UDPSocket::writeData(void *data, int length) {
	if(client != NULL) {
		return writeData(client->ai_addr, client->ai_addrlen, data, length);
	} else {
		printf("Error writing to socket!\n");
		return -1;
	}
}

//Write to arbitrary client
int UDPSocket::writeData(struct sockaddr *remote, socklen_t remoteLength, void *data, int length) {
	//Socket must be open
	if(!open) {
		return -1;
	}

	//Write data to socket
	int tmp = 0;
	if((tmp=sendto(serverFD, IFWIN32((char *))data, length, 0, remote, remoteLength)) < 0) {
		PERROR("Error writing to socket");
		return -1;
	}
	return 0;
}

int UDPSocket::writeData(struct sockaddr_storage *remote, socklen_t remoteLength, void *data, int length) {
	return writeData((struct sockaddr *)remote, remoteLength, data, length);
}
