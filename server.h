#ifndef SERVER_H
#define SERVER_H

//Includes
#include <netinet/in.h>
#include "base.h"

//Declaration
class Server : public Base {
public:
	//Constructor
	Server();

	Server(const char *address, const char *port);

	//Destructor
	~Server();

	//Functions
	void run();
	void sendPing(struct sockaddr_in &remoteAddress);

private:
	//Variables
	struct sockaddr_in robotAddress;
	struct sockaddr_in clientAddress;
	struct sockaddr_in unknownAddress;
};

#endif //SERVER_H
