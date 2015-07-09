#ifndef CLIENT_H
#define CLIENT_H

//Includes
#include <netinet/in.h>
#include "server.h"
#include "mod/input.h"

//Declaration
class Client : public Server {
public:
	//Constructor
	Client();
	Client(const char *address, const char *port, bool dummy = false);
	Client(const char *address, const char *port, int joyNum);
	Client(const char *address, const char *port, char *file);

	//Destructor
	~Client();

	//Functions
	void run();

private:
	//Variables
	bool autodetect;
	Input *input;

	//Functions
	using Server::sendPing;
	void sendPing();
};

#endif //CLIENT_H
