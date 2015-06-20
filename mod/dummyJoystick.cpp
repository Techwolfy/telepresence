//Joystick.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include "dummyJoystick.h"

//Constants
const static int JOY_X_AXIS = 0;
const static int JOY_Y_AXIS = 1;
const static int JOY_Z_AXIS = 2;

//Constructor
DummyJoystick::DummyJoystick() {
	printf("Dummy joystick created.\n");
	numAxes = 3;
	numButtons = 10;

	//Allocate and initialize arrays for dummy joystick data
	axes = (int *)calloc(numAxes, sizeof(int));
	for(int i = 0; i < numAxes; i++) {
		axes[i] = 32767;
	}

	buttons = (bool *)calloc(numButtons, sizeof(bool));
	for(int i = 0; i < numButtons; i++) {
		buttons[i] = true;
	}
}

//Destructor
DummyJoystick::~DummyJoystick() {
	free(axes);
	free(buttons);
}

//Dummy joystick; do nothing
void DummyJoystick::update() {}
