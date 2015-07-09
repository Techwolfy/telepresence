#ifndef SERVER_H
#define SERVER_H

//Includes
#include <netinet/in.h>
#include "telepacket.h"
#include "udpsocket.h"

//Declaration
class Server {
public:
	//Constructor
	Server();
	Server(const char *address, const char *port);

	//Destructor
	virtual ~Server();

	//Functions
	virtual void run();

protected:
	//Variables
	UDPSocket s;
	TelePacket in;
	TelePacket out;
	TelePacket ping;
	struct sockaddr_in robotAddress;
	struct sockaddr_in clientAddress;
	struct sockaddr_in unknownAddress;

	//Functions
	virtual void handlePing();
	virtual void sendPing(struct sockaddr_in &remoteAddress);
	static void printData(TelePacket &data);
};

#endif //SERVER_H
