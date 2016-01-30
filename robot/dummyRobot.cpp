//DummyRobot.cpp

//Includes
#include <stdio.h>
#include "robot/dummyRobot.h"
#include "util/watchdog.h"

//Constructor
DummyRobot::DummyRobot() : ratelimitControl(500),
						   ratelimitStop(500) {
	printf("Dummy robot created.\n");
}

//Destructor
DummyRobot::~DummyRobot() {
	printf("Dummy robot destroyed.\n");
}

//Shared library constructor
extern "C" Output* createRobot(const char *options) {
	return new DummyRobot();
}

//Shared library destructor
extern "C" void destroyRobot(Output *output) {
	delete output;
}

//Functions
//Dummy robot; output values to console
void DummyRobot::control(int numAxes, double axes[], int numButtons, bool buttons[]) {
	if(ratelimitControl.expired()) {
		printf("Dummy robot control() called.\n");
		for(int i = 0; i < numAxes; i++) {
			printf("Axis %d: %f\n", i, axes[i]);
		}
		for(int i = 0; i < numButtons; i++) {
			printf("Button %d: %c\n", i, buttons[i] ? 'T' : 'F');
		}
		ratelimitControl.reset();
	}
}

//Dummy robot; output stop message to console
void DummyRobot::stop() {
	if(ratelimitStop.expired()) {
		printf("Dummy robot stop() called.\n");
		ratelimitStop.reset();
	}
}
