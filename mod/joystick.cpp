//Joystick.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include "joystick.h"

//Constants
const static int JOY_X_AXIS = 0;
const static int JOY_Y_AXIS = 1;
const static int JOY_Z_AXIS = 2;

//Constructor
Joystick::Joystick() : Joystick("/dev/input/js0") {

}

Joystick::Joystick(const char *file) : joyFD(),
								 //joyEvent(default),
								 numAxes(0),
								 numButtons(0),
								 axes(NULL),
								 buttons(NULL) {
	//Set up joystick
		//https://www.kernel.org/doc/Documentation/input/joystick-api.txt
		//http://archives.seul.org/linuxgames/Aug-1999/msg00107.html
	joyFD = open(file, O_NONBLOCK);
	if(joyFD < 0) {
		printf("Error opening joystick!\n");
		//TODO: Throw exception? Probably segfaults on fail right now.
	}
	ioctl(joyFD, JSIOCGAXES, &numAxes);
	ioctl(joyFD, JSIOCGBUTTONS, &numButtons);
	//Allocate and zero-initialize arrays for joystick data
	axes = (int *)calloc(numAxes, sizeof(int));
	buttons = (bool *)calloc(numButtons, sizeof(bool));
}

//Destructor
Joystick::~Joystick() {
	free(axes);
	free(buttons);
	close(joyFD);
}

void Joystick::update() {
	if(read(joyFD, &joyEvent, sizeof(struct js_event)) == -1) {
		return;
	}

	switch(joyEvent.type & ~JS_EVENT_INIT) {	//AND NOT JS_EVENT_INIT (ignore that one)
		case JS_EVENT_AXIS:
			if(joyEvent.number == JOY_X_AXIS) {
				axes[0] = joyEvent.value;
			} else if(joyEvent.number == JOY_Y_AXIS) {
				axes[1] = joyEvent.value;
			} else if(joyEvent.number == JOY_Z_AXIS) {
				axes[2] = joyEvent.value;
			}
			break;
		case JS_EVENT_BUTTON:
			buttons[joyEvent.number] = joyEvent.value ? true : false;	//TODO: Test!
			break;
	}
}

int Joystick::getNumAxes() {
	return numAxes;
}

int Joystick::getNumButtons() {
	return numButtons;
}

double Joystick::getX() {
	update();
	if(numAxes > 1) {
		return axes[0] / SHRT_MAX;	//TODO:  Implicit conversion to `short int` on these?
	} else {
		return 0.0;
	}
}

double Joystick::getY() {
	update();
	if(numAxes > 2) {
		return axes[1] / SHRT_MAX;
	} else {
		return 0.0;
	}
}

double Joystick::getZ() {
	update();
	if(numAxes > 3) {
		return axes[2] / SHRT_MAX;
	} else {
		return 0.0;
	}
}

double Joystick::getAxis(int axis) {
	update();
	if(numAxes > axis - 1) {
		return axes[axis];
	} else {
		return 0.0;
	}
}

bool Joystick::getButton(int button) {
	update();
	if(numButtons > button - 1) {
		return buttons[button];
	} else {
		return false;
	}
}