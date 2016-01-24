#ifndef CLIENT_H
#define CLIENT_H

//Includes
#include "server.h"
#include "util/watchdog.h"
#include "input/input.h"

//Declaration
class Client : public Server {
public:
	//Constructor
	Client();
	Client(const char *address, const char *port, bool listen, bool dummy = false);
	Client(const char *address, const char *port, bool listen, int joyNum);
	Client(const char *address, const char *port, bool listen, char *file);

	//Destructor
	~Client();

	//Functions
	void run();

private:
	//Variables
	Input *input;

	//Functions
	using Server::sendPing;
	void sendPing();
};

#endif //CLIENT_H
