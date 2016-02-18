#ifndef ROBOT_H
#define ROBOT_H

//Includes
#include "server.h"
#include "util/watchdog.h"
#include "robot/robotInterface.h"

//Declaration
class Robot : public Server {
public:
	//Constructor
	Robot();
	Robot(const char *address, const char *port, const char *key, bool listen);
	Robot(const char *address, const char *port, const char *key, bool listen, const char *libFile);
	Robot(const char *address, const char *port, const char *key, bool listen, const char *libFile, const char *libOptions);

	//Destructor
	~Robot();

	//Functions
	void run();

private:
	//Variables
	Watchdog watchdog;
	void *robotLibrary;
	RobotInterface *interface;
	unsigned int axesSize;
	unsigned int buttonsSize;
	double *axes;
	bool *buttons;

	//Functions
	using Server::sendPing;
	void loadRobotLibrary(const char *filename);
	void sendPing();

	//Shared library functions
	RobotInterface *(*createInterface)(const char *options);
	void (*destroyInterface)(RobotInterface *interface);
};

#endif //ROBOT_H
