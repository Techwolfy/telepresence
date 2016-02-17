//Robot.cpp

//Includes
#include <stdio.h>
#include <dlfcn.h>
#include <chrono>
#include <string>
#include <stdexcept>
#include <jsoncpp/json/json.h>
#include "robot.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"
#include "robot/robotInterface.h"
#include "robot/dummyRobot.h"

//Constructor
Robot::Robot() : Robot("127.0.0.1", "8353", NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen) : Robot(address, port, listen, NULL, "") {

}

Robot::Robot(const char *address, const char *port, bool listen, const char *libFile) : Robot(address, port, listen, libFile, NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen, const char *libFile, const char *libOptions) : Server(address, port, listen),
																												watchdog(500, false),
																												robotLibrary(NULL),
																												interface(NULL),
																												axesSize(0),
																												buttonsSize(0),
																												axes(NULL),
																												buttons(NULL) {
	//Set up robot interface
	if(libFile != NULL) {
		loadRobotLibrary(libFile);
	} else {
		createInterface = &createRobot;
		destroyInterface = &destroyRobot;
	}
	interface = createInterface(libOptions);

	//Set up output packet
	out["frameNum"] = 0;
	out["isClient"] = false;
	out["isRobot"] = true;
	out["ping"] = false;
	out["time"] = 0;

	//Set up ping packet
	ping["frameNum"] = 0;
	ping["isClient"] = false;
	ping["isRobot"] = true;
	ping["ping"] = true;
	ping["time"] = 0;

	//Send initialization ping
	sendPing();
}

//Destructor
Robot::~Robot() {
	destroyInterface(interface);
	if(robotLibrary != NULL) {
		dlclose(robotLibrary);
	}
	delete[] axes;
	delete[] buttons;
}

//Functions
//Load library for robot I/O at runtime
void Robot::loadRobotLibrary(const char *filename) {
	//Load library
	robotLibrary = dlopen(filename, RTLD_LAZY);
	if(!robotLibrary) {
		printf("Failed to load robot library: %s\n", dlerror());
		throw std::runtime_error("failed to load robot library");
	} else {
		printf("Loaded robot library: %s\n", filename);
	}

	//Set up robot constructor
	const char* symbolError = NULL;
	*(void**)(&createInterface) = dlsym(robotLibrary, "createRobot");
	symbolError = dlerror();
	if(symbolError) {
		printf("Failed to load robot constructor: %s\n", dlerror());
		throw std::runtime_error("failed to load robot constructor");
	}

	//Set up robot destructor
	*(void**)(&destroyInterface) = dlsym(robotLibrary, "destroyRobot");
	symbolError = dlerror();
	if(symbolError) {
		printf("Failed to load robot destructor: %s\n", dlerror());
		throw std::runtime_error("failed to load robot destructor");
	}
}

//Main robot loop
void Robot::run() {
	//Check the watchdog timer
	if(!watchdog.isAlive()) {
		interface->stop();
	}

	//Display last received data and send a ping once every ~500ms
	//Don't display pings, just wait for the next loop
	if(!keepalive.isAlive() && !in.get("ping", false).asBool()) {
		if(watchdog.isAlive()) {
			printData(in);
		}
		sendPing();
		keepalive.feed();
	}

	//Get control data from server
	buffer[0] = '\0';
	if(s.readData((void *)buffer, sizeof(buffer), &unknownAddress) < 0 || buffer[0] == '\0') {
		return;
	} else {
		reader.parse(buffer, in, false);
		watchdog.feed();
	}

	//Handle pings
	if(in.get("ping", false).asBool()) {
		handlePing();
	}

	//Data recieved from client
	if(in.get("isClient", false).asBool() && watchdog.isAlive()) {
		if(in["axes"].size() > 0 && in["axes"].size() != axesSize) {
			axesSize = in["axes"].size();
			delete[] axes;
			axes = new double[axesSize];
		}
		if(in["buttons"].size() > 0 && in["buttons"].size() != buttonsSize) {
			buttonsSize = in["buttons"].size();
			delete[] buttons;
			buttons = new bool[buttonsSize];
		}
		for(unsigned int i = 0; i < axesSize; i++) {
			axes[i] = in["axes"].get(i, 0.0).asDouble();
		}
		for(unsigned int i = 0; i < buttonsSize; i++) {
			buttons[i] = in["buttons"].get(i, false).asBool();
		}
		interface->run(axesSize, axes, buttonsSize, buttons);
	}
}

//Ping the server
void Robot::sendPing() {
	ping["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	if(listening) {
		sendPing(clientAddress);
	} else {
		std::string pingJSON = writer.write(ping);
		s.writeData((void *)pingJSON.c_str(), pingJSON.length());
	}
	ping["frameNum"] = ping.get("frameNum", 0).asUInt() + 1;
}
