//DummyRobot.cpp

//Includes
#include <stdio.h>
#include "robot/dummyRobot.h"
#include "util/ratelimit.h"

//Constructor
DummyRobot::DummyRobot() : messages(500, 1) {
	runRateID = messages.getID();
	stopRateID = messages.getID();

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
	if(!messages.limitReached(runRateID)) {
		printf("Dummy robot run() called.\n");
		for(int i = 0; i < numAxes; i++) {
			printf("Axis %d: %f\n", i, axes[i]);
		}
		for(int i = 0; i < numButtons; i++) {
			printf("Button %d: %c\n", i, buttons[i] ? 'T' : 'F');
		}
		messages.increment(runRateID);
	}
}

//Dummy robot; output stop message to console
void DummyRobot::stop() {
	if(!messages.limitReached(stopRateID)) {
		printf("Dummy robot stop() called.\n");
		messages.increment(stopRateID);
	}
}
