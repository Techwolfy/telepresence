//DummyRobot.cpp

//Includes
#include <stdio.h>
#include "robot/dummyRobot.h"

//Constructor
DummyRobot::DummyRobot() {
	printf("Dummy robot created.\n");
}

//Destructor
DummyRobot::~DummyRobot() {
	printf("Dummy robot destroyed.\n");
}

//Shared library constructor
extern "C" Output* createRobot() {
	return new DummyRobot();
}

//Shared library destructor
extern "C" void destroyRobot(Output *output) {
	delete output;
}

//Functions
//Dummy robot; output values to console
void DummyRobot::control(int numAxes, double axes[], int numButtons, bool buttons[]) {
	printf("Dummy robot control() called.\n");
	for(int i = 0; i < numAxes; i++) {
		printf("Axis %d: %f\n", i, axes[i]);
	}
	for(int i = 0; i < numButtons; i++) {
		printf("Button %d: %c\n", i, buttons[i]);
	}
}

//Dummy robot; output stop message to console
void DummyRobot::stop() {
	printf("Dummy robot stop() called.\n");
}
