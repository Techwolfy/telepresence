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
#include "output/output.h"
#include "robot/dummyRobot.h"

//Constructor
Robot::Robot() : Robot("127.0.0.1", "8353", NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen) : Robot(address, port, listen, NULL) {

}

Robot::Robot(const char *address, const char *port, bool listen, const char *libFile) : Server(address, port, listen),
																						watchdog(500, false),
																						outputLibrary(NULL),
																						output(NULL),
																						axesSize(0),
																						buttonsSize(0),
																						axes(NULL),
																						buttons(NULL) {
	//Set up output object
	if(libFile != NULL) {
		loadOutputLibrary(libFile);
	} else {
		createOutput = &createRobot;
		destroyOutput = &destroyRobot;
	}
	output = createOutput();

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
	destroyOutput(output);
	if(outputLibrary != NULL) {
		dlclose(outputLibrary);
	}
	delete[] axes;
	delete[] buttons;
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
		for(int i = 0; i < axesSize; i++) {
			axes[i] = in["axes"].get(i, 0.0).asDouble();
		}
		for(int i = 0; i < buttonsSize; i++) {
			buttons[i] = in["buttons"].get(i, false).asBool();
		}
		output->control(axesSize, axes, buttonsSize, buttons);
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
