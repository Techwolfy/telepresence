#ifndef CLIENT_H
#define CLIENT_H

//Includes
#include "server.h"
#include "util/watchdog.h"
#include "control/controller.h"

//Declaration
class Client : public Server {
public:
	//Constructor
	Client();
	Client(const char *address, const char *port, const char *key, bool listen, bool dummy = false);
	Client(const char *address, const char *port, const char *key, bool listen, int joyNum);
	Client(const char *address, const char *port, const char *key, bool listen, char *file);

	//Destructor
	~Client();

	//Functions
	void run();

private:
	//Variables
	Controller *controller;

	//Functions
	using Server::sendPing;
	void sendPing();
};

#endif //CLIENT_H
