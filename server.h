#ifndef SERVER_H
#define SERVER_H

//Includes
#include <netinet/in.h>
#include "telepacket.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"

//Declaration
class Server {
public:
	//Constructor
	Server();
	Server(const char *address, const char *port, bool listen = true);

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
	bool listening;
	Watchdog keepalive;

	//Functions
	virtual void handlePing();
	virtual void sendPing(struct sockaddr_in &remoteAddress);
	static void printData(TelePacket &data);
};

#endif //SERVER_H
