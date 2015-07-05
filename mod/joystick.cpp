//Joystick.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include "joystick.h"

//Constants
const int Joystick::JOY_X_AXIS = 0;
const int Joystick::JOY_Y_AXIS = 1;
const int Joystick::JOY_Z_AXIS = 2;

//Constructor
Joystick::Joystick() : Joystick(0) {

}

Joystick::Joystick(int joyNum) : joyFD(),
								 //joyEvent(default),
								 numAxes(0),
								 numButtons(0),
								 axes(NULL),
								 buttons(NULL),
								 name{0} {
	char joyFile[15] = "/dev/input/js0";
	if(joyNum >= 0 && joyNum < 10) {
		sprintf(joyFile, "/dev/input/js%d", joyNum);
	} else {
		printf("Error identifying joystick!\n");
		throw std::runtime_error("couldn't identify joystick");
	}

	//Set up joystick
		//https://www.kernel.org/doc/Documentation/input/joystick-api.txt
		//http://archives.seul.org/linuxgames/Aug-1999/msg00107.html
	joyFD = open(joyFile, O_RDONLY | O_NONBLOCK);
	if(joyFD < 0) {
		printf("Joystick initialization failed!\n");
		throw std::runtime_error("joystick initialization failed");
	}
	ioctl(joyFD, JSIOCGAXES, &numAxes);
	ioctl(joyFD, JSIOCGBUTTONS, &numButtons);
	if(ioctl(joyFD, JSIOCGNAME(sizeof(name)), name) < 0) {
		strncpy(name, "Unknown Joystick", sizeof(name));
	}
	//Allocate and zero-initialize arrays for joystick data
	axes = (int *)calloc(numAxes, sizeof(int));
	buttons = (bool *)calloc(numButtons, sizeof(bool));

	printf("Initialized joystick: %s\n", name);
}

//Destructor
Joystick::~Joystick() {
	free(axes);
	free(buttons);
	close(joyFD);
}

//Update cached joystick values if anything has changed
void Joystick::update() {
	//Loop through all events in queue
	while(read(joyFD, &joyEvent, sizeof(struct js_event)) > 0) {
		switch(joyEvent.type & ~JS_EVENT_INIT) {	//AND NOT JS_EVENT_INIT (ignore that one)
			case JS_EVENT_AXIS:
				if(joyEvent.number == JOY_X_AXIS) {
					axes[JOY_X_AXIS] = joyEvent.value;
				} else if(joyEvent.number == JOY_Y_AXIS) {
					axes[JOY_Y_AXIS] = joyEvent.value;
				} else if(joyEvent.number == JOY_Z_AXIS) {
					axes[JOY_Z_AXIS] = joyEvent.value;
				}
				break;
			case JS_EVENT_BUTTON:
				buttons[joyEvent.number] = (joyEvent.value == 1);
				break;
		}
	}

	if(errno != EAGAIN) {
		printf("Error reading from joystick!\n");
	}
}

//Retrieve the number of axes on the connected joystick
int Joystick::getNumAxes() {
	return numAxes;
}

//Retrieve the number of buttons on the connected joystick
int Joystick::getNumButtons() {
	return numButtons;
}

//Retrieve the value of a specific axis of the connected joystick
double Joystick::getAxis(int axis) {
	if(axis < numAxes) {
		return -(double)axes[axis] / SHRT_MAX;
	} else {
		return 0.0;
	}
}

//Retrieve the state of a specific button of the connected joystick
bool Joystick::getButton(int button) {
	if(button < numButtons) {
		return buttons[button];
	} else {
		return false;
	}
}
