//BasicRobot.cpp

//Includes
#include <stdio.h>
#include "robot/basicRobot.h"
#include "output/motor.h"
#ifdef POLOLU
	#include "output/pololu.h"
#elif RASPI
	#include "output/raspi.h"
#elif PARALLAX
	#include "output/parallax.h"
#elif ARDUINO
	#include "output/arduino.h"
#else
	#include "output/dummyMotor.h"
#endif

//Constructors
BasicRobot::BasicRobot() {
	//Set up motor
	#ifdef POLOLU
		motor = new Pololu();
	#elif RASPI
		motor = new RasPi();
	#elif PARALLAX
		motor = new Parallax();
	#elif ARDUINO
		motor = new Arduino();
	#else
		motor = new DummyMotor();
	#endif
}

BasicRobot::BasicRobot(const char *deviceFile) {
	printf("Output device file: %s\n", deviceFile);
	//Set up motor
	#ifdef POLOLU
		motor = new Pololu(deviceFile);
	#elif RASPI
		motor = new RasPi();
	#elif PARALLAX
		motor = new Parallax(deviceFile);
	#elif ARDUINO
		motor = new Arduino(deviceFile);
	#else
		motor = new DummyMotor();
	#endif
}

//Destructor
BasicRobot::~BasicRobot() {
	delete motor;
}

//Shared library constructor
extern "C" Output* createRobot(const char *options) {
	if(options != NULL) {
		return new BasicRobot(options);
	} else {
		return new BasicRobot();
	}
}

//Shared library destructor
extern "C" void destroyRobot(Output *output) {
	delete output;
}

//Functions
//Run motors
void BasicRobot::control(int numValues, double values[], int numButtons, bool buttons[]) {
	motor->control(numValues, values);
	//Buttons are currently ignored
}

//Stop motors
void BasicRobot::stop() {
	motor->stop();
}
