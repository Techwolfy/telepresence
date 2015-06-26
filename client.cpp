//Client.cpp

//Includes
#include <stdio.h>
#include "client.h"
#include "udpsocket.h"
#include "mod/input.h"
#ifdef JOYSTICK
	#include "mod/joystick.h"
#else
	#include "mod/dummyJoystick.h"
#endif

//Constructor
Client::Client() : Client("127.0.0.1", "8353") {

}

Client::Client(const char *address, const char *port) : Base(address, port) {
	//Set up joystick
#ifdef JOYSTICK
	joystick = new Joystick();
#else
	joystick = new DummyJoystick();
#endif

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

//Destructor
Client::~Client() {
	delete joystick;
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
	for(int i = 0; i < joystick->getNumAxes() && i < TelePacket::NUM_AXES; i++) {
		out.axes[i] = joystick->getAxis(i);
	}
	for(int i = 0; i < joystick->getNumButtons() && i < TelePacket::NUM_BUTTONS; i++) {
		out.buttons[i] = joystick->getButton(i);
	}

	//Send data to robot
	s.writeData((void *)&out, sizeof(out));
}

//Ping the server
void Client::sendPing() {
	s.writeData((void *)&ping, sizeof(ping));
	ping.frameNum++;
}
