#ifndef SERVER_H
#define SERVER_H

//Includes
#ifndef _WIN32
	#include <sys/socket.h>
#else
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#include <jsoncpp/json/json.h>
#include "util/udpsocket.h"
#include "util/watchdog.h"

//Declaration
class Server {
public:
	//Constructor
	Server();
	Server(const char *address, const char *port, const char *key, bool listen = true);

	//Destructor
	virtual ~Server();

	//Functions
	virtual void run();

protected:
	//Variables
	UDPSocket s;
	char buffer[1024];
	Json::Reader reader;
	Json::FastWriter writer;
	Json::Value in;
	Json::Value out;
	Json::Value ping;
	struct sockaddr_storage unknownAddress;
	struct sockaddr_storage clientAddress;
	struct sockaddr_storage robotAddress;
	socklen_t unknownAddressLength;
	socklen_t clientAddressLength;
	socklen_t robotAddressLength;
	const char *password;
	bool listening;
	Watchdog keepalive;

	//Functions
	virtual void handlePing();
	virtual void sendPing(struct sockaddr_storage *remoteAddress, socklen_t remoteAddressLength);
	virtual bool validateKey(Json::Value &data);
	static void printData(Json::Value &data);
	static void printLatency(Json::Value &data);
};

#endif //SERVER_H
