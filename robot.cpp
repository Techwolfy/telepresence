//Robot.cpp

//Includes
#include <stdio.h>
#include <dlfcn.h>
#include <stdexcept>
#include "robot.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"
#include "output/output.h"
#include "robot/dummyRobot.h"

//Constructor
Robot::Robot() : Robot("127.0.0.1", "8353", NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen) : Robot(address, port, listen, NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen, const char *libFile) : Server(address, port, listen),
																						watchdog(500),
																						outputLibrary(NULL),
																						output(NULL) {
	//Set up output object
	if(libFile != NULL) {
		loadOutputLibrary(libFile);
	} else {
		createOutput = &createRobot;
		destroyOutput = &destroyRobot;
	}
	output = createOutput();

	//Set up output packet
	out.frameNum = 0;
	out.isClient = false;
	out.isRobot = true;
	out.ping = false;

	//Set up ping packet
	ping.frameNum = 0;
	ping.isClient = false;
	ping.isRobot = true;
	ping.ping = true;

	//Send initialization ping
	sendPing();
}

//Destructor
Robot::~Robot() {
	destroyOutput(output);
	if(outputLibrary == NULL) {
		dlclose(outputLibrary);
	}
}

//Functions
//Load library for outputs at runtime
void Robot::loadOutputLibrary(const char *filename) {
	//Load library
	outputLibrary = dlopen(filename, RTLD_LAZY);
	if(!outputLibrary) {
		printf("Failed to load output library: %s\n", dlerror());
		throw std::runtime_error("failed to load output library");
	} else {
		printf("Loaded output library: %s\n", filename);
	}

	//Set up output constructor
	const char* symbolError = NULL;
	*(void**)(&createOutput) = dlsym(outputLibrary, "createRobot");
	symbolError = dlerror();
	if(symbolError) {
		printf("Failed to load output constructor: %s\n", dlerror());
		throw std::runtime_error("failed to load output constructor");
	}

	//Set up output destructor
	*(void**)(&destroyOutput) = dlsym(outputLibrary, "destroyRobot");
	symbolError = dlerror();
	if(symbolError) {
		printf("Failed to load output destructor: %s\n", dlerror());
		throw std::runtime_error("failed to load output destructor");
	}
}

//Main robot loop
void Robot::run() {
	//Check the watchdog timer
	if(!watchdog.isAlive()) {
		output->stop();
	}

	//Get control data from server
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in)) < 0 || in.head != 'T') {
		return;
	} else {
		watchdog.feed();
	}

	//Handle pings
	if(in.ping) {
		handlePing();
	}

	//Data recieved from client
	if(in.isClient && watchdog.isAlive()) {
		printData(in);
		output->control(TelePacket::NUM_AXES, in.axes, TelePacket::NUM_BUTTONS, in.buttons);
	}
}

//Ping the server
void Robot::sendPing() {
	if(listening) {
		sendPing(clientAddress);
	} else {
		s.writeData((void *)&ping, sizeof(ping));
	}
	ping.frameNum++;
}
