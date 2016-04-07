#ifndef UDPSOCKET_H
#define UDPSOCKET_H

//Includes
#ifndef _WIN32	//Linux/POSIX support
	#include <sys/socket.h>
	#include <netdb.h>
#else			//Windows support
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif

//Declaration
class UDPSocket {
public:
	//Constructor
	UDPSocket();

	//Destructor
	~UDPSocket();

	//Functions
	int openSocket(const char *localAddress, const char *localPort, const char *remoteAddress, const char *remotePort);
	void closeSocket();
	bool isOpen();
	int blockRead(bool block);
	int setRecieveBufferLength(int length);
	int readData(void *data, int length);
	int readData(void *data, int length, struct sockaddr *remote, socklen_t *remoteLength);
	int readData(void *data, int length, struct sockaddr_storage *remote, socklen_t *remoteLength);
	int writeData(void *data, int length);
	int writeData(struct sockaddr *remote, socklen_t remoteLength, void *data, int length);
	int writeData(struct sockaddr_storage *remote, socklen_t remoteLength, void *data, int length);

private:
	//Variables
	bool open;
	int serverFD;
	int readFlags;
	struct addrinfo *server;
	struct addrinfo *client;

	//Functions
	static char* getAddrString(struct sockaddr *addr, socklen_t addrLength, char *buffer, int length);
	int openSocket(struct addrinfo *localAddr, struct addrinfo *remoteAddr);
};

#endif //UDPSOCKET_H
