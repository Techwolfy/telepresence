//DummyRobot.cpp

//Includes
#include <stdio.h>
#include "robot/dummyRobot.h"
#include "util/watchdog.h"

//Constructor
DummyRobot::DummyRobot() : ratelimitRun(500),
						   ratelimitStop(500) {
	printf("Dummy robot created.\n");
}

//Destructor
DummyRobot::~DummyRobot() {
	printf("Dummy robot destroyed.\n");
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	return new DummyRobot();
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Dummy robot; output values to console
void DummyRobot::run(int numAxes, double axes[], int numButtons, bool buttons[]) {
	if(ratelimitRun.expired()) {
		printf("Dummy robot run() called.\n");
		for(int i = 0; i < numAxes; i++) {
			printf("Axis %d: %f\n", i, axes[i]);
		}
		for(int i = 0; i < numButtons; i++) {
			printf("Button %d: %c\n", i, buttons[i] ? 'T' : 'F');
		}
		ratelimitRun.reset();
	}
}

//Dummy robot; output stop message to console
void DummyRobot::stop() {
	if(ratelimitStop.expired()) {
		printf("Dummy robot stop() called.\n");
		ratelimitStop.reset();
	}
}
