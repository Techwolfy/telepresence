//Joystick.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdexcept>
#ifndef _WIN32
	#include <sys/ioctl.h>
	#include <linux/joystick.h>
#else
	#include <windows.h>
	#include <mmsystem.h>
#endif
#include "control/joystick.h"
#include "util/log.h"

//Constructor
Joystick::Joystick() : Joystick(0) {

}

#ifndef _WIN32
Joystick::Joystick(int joyNum) : joyFD(-1),
#else
Joystick::Joystick(int joyNum) : joyFD(joyNum),
#endif
								 joyEvent{0},
								 numAxes(0),
								 numButtons(0),
								 axes(NULL),
								 buttons(NULL),
								 name{0} {
#ifndef _WIN32
	char joyFile[15] = "/dev/input/js0";
	if(joyNum >= 0 && joyNum < 10) {
		snprintf(joyFile, sizeof(joyFile), "/dev/input/js%d", joyNum);
	} else {
		Log::logf(Log::ERR, "Error identifying joystick!\n");
		throw std::runtime_error("couldn't identify joystick");
	}

	//Set up joystick
		//https://www.kernel.org/doc/Documentation/input/joystick-api.txt
		//http://archives.seul.org/linuxgames/Aug-1999/msg00107.html
	joyFD = open(joyFile, O_RDONLY | O_NONBLOCK);
	if(joyFD < 0) {
		Log::logf(Log::ERR, "Joystick initialization failed!\n");
		throw std::runtime_error("joystick initialization failed");
	}

	//Load joystick info
	ioctl(joyFD, JSIOCGAXES, &numAxes);
	ioctl(joyFD, JSIOCGBUTTONS, &numButtons);
	if(ioctl(joyFD, JSIOCGNAME(sizeof(name)), name) < 0) {
		strncpy(name, "Unknown Joystick", sizeof(name));
	}
#else
	//Set up joystick
		//https://msdn.microsoft.com/en-us/library/vs/alm/dd757116%28v=vs.85%29.aspx
	if(joyGetNumDevs() < 1) {
		Log::logf(Log::ERR, "Error identifying joystick!\n");
		throw std::runtime_error("couldn't identify joystick");
	}
	if(joyGetPosEx(joyNum, &joyEvent) < 0) {
		Log::logf(Log::ERR, "Error finding joystick!\n");
		throw std::runtime_error("couldn't find joystick");
	}

	//Load joystick info
	JOYCAPS capabilities = {0};
	if(joyGetDevCaps(joyNum, &capabilities, sizeof(capabilities)) < 0) {
		Log::logf(Log::ERR, "Error retreiving joystick capabilities!\n");
		throw std::runtime_error("couldn't retreive joystick capabilities");
	}
	numAxes = capabilities.wNumAxes;
	numButtons = capabilities.wNumButtons;
	strncpy(name, capabilities.szPname, sizeof(name));

	//Set up joyEvent
	joyEvent.dwSize = sizeof(JOYINFO);
	joyEvent.dwFlags = JOY_RETURNALL;
#endif

	//Allocate and zero-initialize arrays for joystick data
	axes = (int *)calloc(numAxes, sizeof(int));
	buttons = (bool *)calloc(numButtons, sizeof(bool));

	Log::logf(Log::INFO, "Initialized joystick: %s\n", name);
}

//Destructor
Joystick::~Joystick() {
	free(axes);
	free(buttons);
#ifndef _WIN32
	close(joyFD);
#else
	//WinMM (Windows MultiMedia joystick API) is used statelessly, so no cleanup is necessary
#endif
}

//Functions
//Update cached joystick values if anything has changed
void Joystick::update() {
#ifndef _WIN32
	//Loop through all events in queue
	while(read(joyFD, &joyEvent, sizeof(struct js_event)) > 0) {
		switch(joyEvent.type & ~JS_EVENT_INIT) {	//AND NOT JS_EVENT_INIT (ignore that one)
			case JS_EVENT_AXIS:
				if(joyEvent.number == 0) {
					axes[0] = joyEvent.value;
				} else if(joyEvent.number == 1) {
					axes[1] = joyEvent.value;
				} else if(joyEvent.number == 2) {
					axes[2] = joyEvent.value;
				}
				break;
			case JS_EVENT_BUTTON:
				buttons[joyEvent.number] = (joyEvent.value == 1);
				break;
		}
	}

	if(errno != EAGAIN) {
		Log::logf(Log::WARN, "Error reading from joystick!\n");
	}
#else
	//Retrieve current joystick state
	if(joyGetPosEx(joyFD, &joyEvent) < 0) {
		Log::logf(Log::WARN, "Error reading from joystick!\n");
	} else {
		//Update values of all present axes
		switch(numAxes) {
			case 6:
				axes[5] = joyEvent.dwVpos;
			case 5:
				axes[4] = joyEvent.dwUpos;
			case 4:
				axes[3] = joyEvent.dwRpos;
			case 3:
				axes[2] = joyEvent.dwZpos;
			case 2:
				axes[1] = joyEvent.dwYpos;
			case 1:
				axes[0] = joyEvent.dwXpos;
			default:
				break;
		}

		//Separate and update button values via bitmask
		for(unsigned long i = 0x1; i <= 0x80000000 && i != 0x0; i <<= 1) {	//Bit-shift once for each button, end after either bit 32 or integer rollover
			buttons[i] = joyEvent.dwButtons & i;
		}
	}
#endif
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
