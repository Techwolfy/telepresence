//Client.cpp

//Includes
#include <stdio.h>
#include "client.h"
#include "udpsocket.h"
#include "mod/input.h"
#include "mod/dummyJoystick.h"
#include "mod/joystick.h"
#include "mod/controlFile.h"

//Constructor
Client::Client() : Client("127.0.0.1", "8353") {

}

Client::Client(const char *address, const char *port, bool dummy /* = false */) : Base(address, port) {
	//Set up dummy joystick if necessary
	if(dummy) {
		input = new DummyJoystick();
	}

	//Don't block on read, data still needs to be sent to the robot
	s.blockRead(false);

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
	if(s.readData((void *)&in, sizeof(in)) > 0 && in.head == 'T') {
		if(in.ping) {
			printf("Ping %d recieved!\n", in.frameNum);
		}
	}

	//Prepare joystick data for robot
	out.frameNum++;
	for(int i = 0; i < input->getNumAxes() && i < TelePacket::NUM_AXES; i++) {
		out.axes[i] = input->getAxis(i);
	}
	for(int i = 0; i < input->getNumButtons() && i < TelePacket::NUM_BUTTONS; i++) {
		out.buttons[i] = input->getButton(i);
	}

	//Send data to robot
	s.writeData((void *)&out, sizeof(out));
}

//Ping the server
void Client::sendPing() {
	s.writeData((void *)&ping, sizeof(ping));
	ping.frameNum++;
}
