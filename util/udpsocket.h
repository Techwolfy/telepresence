#ifndef UDPSOCKET_H
#define UDPSOCKET_H

//Includes
#ifndef _WIN32	//Linux/POSIX support
	#include <sys/socket.h>
	#include <netinet/in.h>
#else			//Windows support
	#include <winsock2.h>
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
	int readData(void *data, int length, sockaddr_in *remote);
	int writeData(void *data, int length);
	int writeData(sockaddr_in *remote, void *data, int length);

private:
	//Variables
	bool open;
	int serverFD;
	int readFlags;
	struct sockaddr_in server;
	struct sockaddr_in client;

	//Functions
	int openSocket(unsigned long localAddress, int localPort, unsigned long remoteAddress, int remotePort);
};

#endif //UDPSOCKET_H
