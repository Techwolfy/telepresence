//Robot.cpp

//Includes
#ifndef _WIN32
	#include <dlfcn.h>
#else
	#include <windows.h>
#endif
#include <chrono>
#include <string>
#include <stdexcept>
#include <jsoncpp/json/json.h>
#include "robot.h"
#include "util/log.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"
#include "robot/robotInterface.h"
#include "robot/dummyRobot.h"

//Constructor
Robot::Robot() : Robot("127.0.0.1", "8353", "", NULL) {

}

Robot::Robot(const char *address, const char *port, const char *key, bool listen) : Robot(address, port, key, listen, NULL, "") {

}

Robot::Robot(const char *address, const char *port, const char *key, bool listen, const char *libFile) : Robot(address, port, key, listen, libFile, NULL) {

}

Robot::Robot(const char *address, const char *port, const char *key, bool listen, const char *libFile, const char *libOptions) : Server(address, port, key, listen),
																																 watchdog(100, false),	//5 packets
																																 robotLibrary(NULL),
																																 robotInterface(NULL),
																																 axesSize(0),
																																 buttonsSize(0),
																																 axes(NULL),
																																 buttons(NULL) {
	//Set up robot robotInterface
	if(libFile != NULL) {
		loadRobotLibrary(libFile);
	} else {
		createInterface = &createRobot;
		destroyInterface = &destroyRobot;
	}
	robotInterface = createInterface(libOptions);

	//Set up output packet
	out["frameNum"] = 0;
	out["isClient"] = false;
	out["isRobot"] = true;
	out["ping"] = false;
	out["key"] = key;
	out["time"] = 0;

	//Set up ping packet
	ping["frameNum"] = 0;
	ping["isClient"] = false;
	ping["isRobot"] = true;
	ping["ping"] = true;
	ping["key"] = key;
	ping["time"] = 0;

	//Send initialization ping
	sendPing();
}

//Destructor
Robot::~Robot() {
	robotInterface->stop();
	destroyInterface(robotInterface);

#ifndef _WIN32	//Linux/POSIX shared library destructor
	if(robotLibrary != NULL) {
		dlclose(robotLibrary);
	}
#else	//Windows shared library destructor
	FreeLibrary((HMODULE)robotLibrary);
#endif

	delete[] axes;
	delete[] buttons;
}

//Functions
//Load library for robot I/O at runtime
void Robot::loadRobotLibrary(const char *filename) {
#ifndef _WIN32	//Linux/POSIX support
	//Load library
	robotLibrary = dlopen(filename, RTLD_LAZY);
	if(!robotLibrary) {
		Log::logf(Log::ERR, "Failed to load robot library: %s\n", dlerror());
		throw std::runtime_error("failed to load robot library");
	} else {
		Log::logf(Log::INFO, "Loaded robot library: %s\n", filename);
	}

	//Set up robot constructor
	createInterface = (RobotInterface* (*)(const char *))dlsym(robotLibrary, "createRobot");
	if(!createInterface) {
		Log::logf(Log::ERR, "Failed to load robot constructor: %s\n", dlerror());
		throw std::runtime_error("failed to load robot constructor");
	}

	//Set up robot destructor
	destroyInterface = (void (*)(RobotInterface*))dlsym(robotLibrary, "destroyRobot");
	if(!destroyInterface) {
		Log::logf(Log::ERR, "Failed to load robot destructor: %s\n", dlerror());
		throw std::runtime_error("failed to load robot destructor");
	}
#else	//Windows support
	//https://msdn.microsoft.com/en-us/library/ms810279.aspx
	robotLibrary = (/*HMODULE*/ void*)LoadLibrary(filename);

	if(!robotLibrary) {
		Log::logf(Log::ERR, "Failed to load robot library: Win32 error %lu\n", GetLastError());
		throw std::runtime_error("failed to load robot library");
	} else {
		Log::logf(Log::INFO, "Loaded robot library: %s\n", filename);
	}

	//Set up robot contructor
	createInterface = (RobotInterface* (*)(const char *))GetProcAddress((HMODULE)robotLibrary, "createRobot");
	if(!createInterface) {
		Log::logf(Log::ERR, "Failed to load robot constructor: Win32 error %lu\n", GetLastError());
		throw std::runtime_error("failed to load robot constructor");
	}

	//Set up robot destructor
	destroyInterface = (void (*)(RobotInterface*))GetProcAddress((HMODULE)robotLibrary, "destroyRobot");
	if(!destroyInterface) {
		Log::logf(Log::ERR, "Failed to load robot destructor: Win32 error %lu\n", GetLastError());
		throw std::runtime_error("failed to load robot destructor");
	}
#endif
}

//Main robot loop
void Robot::run() {
	//Check the watchdog timer
	if(!watchdog.isAlive()) {
		robotInterface->stop();
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
	unknownAddressLength = sizeof(unknownAddress);
	if(s.readData((void *)buffer, sizeof(buffer), &unknownAddress, &unknownAddressLength) < 0 || buffer[0] == '\0') {
		return;
	} else {
		reader.parse(buffer, in, false);
		if(!validateKey(in)) {
			//Ignore packets with invalid keys
			return;
		}
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
		robotInterface->run(axesSize, axes, buttonsSize, buttons);
	}
}

//Ping the server
void Robot::sendPing() {
	ping["time"] = (Json::Value::Int64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	if(listening) {
		sendPing(&clientAddress, clientAddressLength);
	} else {
		std::string pingJSON = writer.write(ping);
		s.writeData((void *)pingJSON.c_str(), pingJSON.length());
	}
	ping["frameNum"] = ping.get("frameNum", 0).asUInt() + 1;
}
