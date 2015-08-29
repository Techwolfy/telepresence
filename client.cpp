//Client.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "client.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"
#include "input/input.h"
#include "input/dummyJoystick.h"
#include "input/joystick.h"
#include "input/controlFile.h"

//Constructor
Client::Client() : Client("127.0.0.1", "8353", false) {

}

Client::Client(const char *address, const char *port, bool listen, bool dummy /* = false */) : Server(address, port, listen) {
	//Set up dummy joystick if necessary
	if(dummy) {
		input = new DummyJoystick();
	}

	//Set up output packet
	out.frameNum = 0;
	out.isClient = true;
	out.isRobot = false;
	out.ping = false;

	//Set up ping packet
	ping.frameNum = 0;
	ping.isClient = true;
	ping.isRobot = false;
	ping.ping = true;

	//Send initialization ping
	sendPing();
}

Client::Client(const char *address, const char *port, bool listen, int joyNum) : Client(address, port, listen) {
	//Set up joystick
	input = new Joystick(joyNum);
}

Client::Client(const char *address, const char *port, bool listen, char *file) : Client(address, port, listen) {
	//Set up web client pipe
	input = new ControlFile(file);
}

//Destructor
Client::~Client() {
	delete input;
}

//Functions
//Main client loop
void Client::run() {
	//Display last sent data and send a ping once every 500ms
	if(!keepalive.isAlive()) {
		printData(out);
		sendPing();
		keepalive.feed();
	}

	//Respond to pings
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in), &unknownAddress) > 0 && in.head == 'T') {
		if(in.ping) {
			handlePing();
		}
	}

	//Prepare joystick data for robot
	input->update();
	out.frameNum++;
	for(int i = 0; i < input->getNumAxes() && i < TelePacket::NUM_AXES; i++) {
		out.axes[i] = input->getAxis(i);
	}
	for(int i = 0; i < input->getNumButtons() && i < TelePacket::NUM_BUTTONS; i++) {
		out.buttons[i] = input->getButton(i);
	}

	//Send data to robot
	if(listening) {
		s.writeData(&robotAddress, (void *)&out, sizeof(out));
	} else {
		s.writeData((void *)&out, sizeof(out));
	}
}

//Ping the server
void Client::sendPing() {
	if(listening) {
		sendPing(robotAddress);
	} else {
		s.writeData((void *)&ping, sizeof(ping));
	}
	ping.frameNum++;
}
