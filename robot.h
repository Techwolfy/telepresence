#ifndef ROBOT_H
#define ROBOT_H

//Includes
#include "server.h"
#include "util/watchdog.h"
#include "output/output.h"

//Declaration
class Robot : public Server {
public:
	//Constructor
	Robot();
	Robot(const char *address, const char *port, bool listen);
	Robot(const char *address, const char *port, bool listen, const char *libFile);
	Robot(const char *address, const char *port, bool listen, const char *libFile, const char *libOptions);

	//Destructor
	~Robot();

	//Functions
	void run();

private:
	//Variables
	Watchdog watchdog;
	void *outputLibrary;
	Output *output;
	unsigned int axesSize;
	unsigned int buttonsSize;
	double *axes;
	bool *buttons;

	//Functions
	using Server::sendPing;
	void loadOutputLibrary(const char *filename);
	void sendPing();

	//Shared library functions
	Output *(*createOutput)(const char *options);
	void (*destroyOutput)(Output *output);
};

#endif //ROBOT_H
