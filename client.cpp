//Client.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "client.h"
#include "util/udpsocket.h"
#include "input/input.h"
#include "input/dummyJoystick.h"
#include "input/joystick.h"
#include "input/controlFile.h"

//Constructor
Client::Client() : Client("127.0.0.1", "8353") {

}

Client::Client(const char *address, const char *port, bool dummy /* = false */) : Server(address, port),
																				  autodetect(false) {
	//Set up dummy joystick if necessary
	if(dummy) {
		input = new DummyJoystick();
	}

	if(strcmp(address, "0.0.0.0") == 0) {
		autodetect = true;
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

Client::Client(const char *address, const char *port, int joyNum) : Client(address, port) {
	//Set up joystick
	input = new Joystick(joyNum);
}

Client::Client(const char *address, const char *port, char *file) : Client(address, port) {
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
	//Respond to pings
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in), &unknownAddress) > 0 && in.head == 'T') {
		if(in.ping && autodetect) {
			handlePing();
		} else if(in.ping) {
			printf("Ping %d recieved!\n", in.frameNum);
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
	printData(out);
	if(autodetect) {
		s.writeData(&robotAddress, (void *)&out, sizeof(out));
	} else {
		s.writeData((void *)&out, sizeof(out));
	}
}

//Ping the server
void Client::sendPing() {
	if(autodetect) {
		sendPing(robotAddress);
	} else {
		s.writeData((void *)&ping, sizeof(ping));
	}
	ping.frameNum++;
}
