//DummyJoystick.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include "dummyJoystick.h"

//Constants
const int DummyJoystick::JOY_X_AXIS = 0;
const int DummyJoystick::JOY_Y_AXIS = 1;
const int DummyJoystick::JOY_Z_AXIS = 2;
const int DummyJoystick::DUMMY_JOY_AXES = 3;
const int DummyJoystick::DUMMY_JOY_BUTTONS = 10;
const double DummyJoystick::DUMMY_JOY_AXIS_VALUE = 0.5;
const bool DummyJoystick::DUMMY_JOY_BUTTON_VALUE = true;

//Constructor
DummyJoystick::DummyJoystick() {
	printf("Dummy joystick created.\n");
}

//Destructor
DummyJoystick::~DummyJoystick() {
	printf("Dummy joystick destroyed.\n");
}

//Functions
int DummyJoystick::getNumAxes() {
	return DUMMY_JOY_AXES;
}

int DummyJoystick::getNumButtons() {
	return DUMMY_JOY_BUTTONS;
}

double DummyJoystick::getAxis(int axis) {
	if(axis < DUMMY_JOY_AXES) {
		return DUMMY_JOY_AXIS_VALUE;
	} else {
		return 0.0;
	}
}

bool DummyJoystick::getButton(int button) {
	if(button < DUMMY_JOY_BUTTONS) {
		return DUMMY_JOY_BUTTON_VALUE;
	} else {
		return false;
	}
}