//Robot.cpp

//Includes
#include <stdio.h>
#include "robot.h"
#include "udpsocket.h"
#include "mod/motor.h"
#ifdef POLOLU
	#include "mod/pololu.h"
#elif RASPI
	#include "mod/raspi.h"
#else
	#include "mod/dummyMotor.h"
#endif

//Constructor
Robot::Robot() : Robot("127.0.0.1", "8353") {

}

Robot::Robot(const char *address, const char *port) : Base(address, port) {
	//Set up motor
#ifdef POLOLU
	motor = new Pololu();
#elif RASPI
	motor = new RasPi();
#else
	motor = new DummyMotor();
#endif

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
	delete motor;
}

//Functions
//Main robot loop
void Robot::run() {
	//Get control data from server
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in)) < 0 || in.head != 'T') {
		return;
	}
	if(in.ping) {
		printf("Ping %d recieved!\n", in.frameNum);
		return;
	}

	if(in.isClient) {	//Data recieved from client
		printData(in);
		motor->control(TelePacket::NUM_AXES, in.axes);
	}
}

//Ping the server
void Robot::sendPing() {
	s.writeData((void *)&ping, sizeof(ping));
	ping.frameNum++;
}
