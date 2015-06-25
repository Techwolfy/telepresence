#ifndef UDPSOCKET_H
#define UDPSOCKET_H

//Includes
#include <sys/socket.h>
#include <netinet/in.h>

//Declaration
class UDPSocket {
public:
	//Constructor
	UDPSocket();

	//Destructor
	~UDPSocket();

	//Functions
	int openSocket(const char *address, const char *port);
	void closeSocket();
	void blockRead(bool block);
	int readData(void *data, int length);
	int readData(void *data, int length, sockaddr_in *remote);
	void writeData(void *data, int length);
	void writeData(sockaddr_in *remote, void *data, int length);

	//Variables
	const static char *SOCKET_DEFAULT_SERVER_ADDRESS;
	const static char *SOCKET_DEFAULT_CLIENT_ADDRESS;
	const static char *SOCKET_DEFAULT_PORT;

private:
	//Variables
	int serverFD;
	int readFlags;
	struct sockaddr_in server;
	struct sockaddr_in client;

	//Functions
	int openSocket(unsigned long address, int port);

};

#endif //UDPSOCKET_H
