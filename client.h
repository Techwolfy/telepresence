#ifndef CLIENT_H
#define CLIENT_H

//Includes
#include "base.h"
#include "mod/input.h"

//Declaration
class Client : public Base {
public:
	//Constructor
	Client();

	Client(const char *address, const char *port);

	//Destructor
	~Client();

	//Functions
	void run();
	void sendPing();

private:
	//Variables
	Input *joystick;
};

#endif //CLIENT_H
