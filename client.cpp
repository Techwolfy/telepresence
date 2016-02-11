//Client.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <string>
#include <jsoncpp/json/json.h>
#include "client.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"
#include "control/controller.h"
#include "control/dummyJoystick.h"
#include "control/joystick.h"
#include "control/controlFile.h"

//Constructor
Client::Client() : Client("127.0.0.1", "8353", false) {

}

Client::Client(const char *address, const char *port, bool listen, bool dummy /* = false */) : Server(address, port, listen) {
	//Set up dummy joystick if necessary
	if(dummy) {
		controller = new DummyJoystick();
	}

	//Set up output packet
	out["frameNum"] = 0;
	out["isClient"] = true;
	out["isRobot"] = false;
	out["ping"] = false;
	out["time"] = 0;

	//Set up ping packet
	ping["frameNum"] = 0;
	ping["isClient"] = true;
	ping["isRobot"] = false;
	ping["ping"] = true;
	ping["time"] = 0;

	//Send initialization ping
	sendPing();
}

Client::Client(const char *address, const char *port, bool listen, int joyNum) : Client(address, port, listen) {
	//Set up joystick
	controller = new Joystick(joyNum);
}

Client::Client(const char *address, const char *port, bool listen, char *file) : Client(address, port, listen) {
	//Set up web client pipe
	controller = new ControlFile(file);
}

//Destructor
Client::~Client() {
	delete controller;
}

//Functions
//Main client loop
void Client::run() {
	//Display last sent data and send a ping once every 500ms
	if(!keepalive.isAlive()) {
		printData(out);
		sendPing();
		keepalive.feed();
		return;	//Never send more than one packet per cycle
	}

	//Respond to pings
	buffer[0] = '\0';
	if(s.readData((void *)buffer, sizeof(buffer), &unknownAddress) > 0 && buffer[0] != '\0') {
		reader.parse(buffer, in, false);
		if(in.get("ping", false).asBool()) {
			handlePing();
		}
	}

	//Prepare joystick data for robot
	controller->update();
	out["frameNum"] = out.get("frameNum", 0).asUInt() + 1;
	for(int i = 0; i < controller->getNumAxes(); i++) {
		out["axes"][i] = controller->getAxis(i);
	}
	for(int i = 0; i < controller->getNumButtons(); i++) {
		out["buttons"][i] = controller->getButton(i);
	}
	out["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	//Send data to robot
	std::string outJSON = writer.write(out);
	if(listening) {
		s.writeData(&robotAddress, (void *)outJSON.c_str(), outJSON.length());
	} else {
		s.writeData((void *)outJSON.c_str(), outJSON.length());
	}
}

//Ping the server
void Client::sendPing() {
	ping["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	if(listening) {
		sendPing(robotAddress);
	} else {
		std::string pingJSON = writer.write(ping);
		s.writeData((void *)pingJSON.c_str(), pingJSON.length());
	}
	ping["frameNum"] = ping.get("frameNum", 0).asUInt() + 1;
}
