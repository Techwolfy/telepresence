#ifndef ROBOT_H
#define ROBOT_H

//Includes
#include "server.h"
#include "mod/motor.h"
#include "mod/watchdog.h"

//Declaration
class Robot : public Server {
public:
	//Constructor
	Robot();
	Robot(const char *address, const char *port);

	//Destructor
	~Robot();

	//Functions
	void run();

private:
	//Variables
	Motor *motor;
	Watchdog watchdog;

	//Functions
	void sendPing();
};

#endif //ROBOT_H
