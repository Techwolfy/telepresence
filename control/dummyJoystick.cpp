//DummyJoystick.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include "control/dummyJoystick.h"
#include "util/ratelimit.h"

//Constants
const int DummyJoystick::JOY_X_AXIS = 0;
const int DummyJoystick::JOY_Y_AXIS = 1;
const int DummyJoystick::JOY_Z_AXIS = 2;
const int DummyJoystick::DUMMY_JOY_AXES = 3;
const int DummyJoystick::DUMMY_JOY_BUTTONS = 10;
const double DummyJoystick::DUMMY_JOY_AXIS_VALUE = 0.5;
const bool DummyJoystick::DUMMY_JOY_BUTTON_VALUE = true;

//Constructor
DummyJoystick::DummyJoystick() : messages(500, 1) {
	updateRateID = messages.getID();

	printf("Dummy joystick created.\n");
}

//Destructor
DummyJoystick::~DummyJoystick() {
	printf("Dummy joystick destroyed.\n");
}

//Functions
//"Update" the static dummy joystick values
void DummyJoystick::update() {
	if(messages.limitReached(updateRateID)) {
		printf("Dummy joystick updated!\n");
		messages.increment(updateRateID);
	}
}

//Retrieve the number of axes on the dummy joystick
int DummyJoystick::getNumAxes() {
	return DUMMY_JOY_AXES;
}

//Retrieve the number of buttons on the dummy joystick
int DummyJoystick::getNumButtons() {
	return DUMMY_JOY_BUTTONS;
}

//Retrieve the value of a specific axis of the dummy joystick
double DummyJoystick::getAxis(int axis) {
	if(axis < DUMMY_JOY_AXES) {
		return DUMMY_JOY_AXIS_VALUE;
	} else {
		return 0.0;
	}
}

//Retrieve the state of a specific button of the dummy joystick
bool DummyJoystick::getButton(int button) {
	if(button < DUMMY_JOY_BUTTONS) {
		return DUMMY_JOY_BUTTON_VALUE;
	} else {
		return false;
	}
}