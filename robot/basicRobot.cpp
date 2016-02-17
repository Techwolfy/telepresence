//BasicRobot.cpp

//Includes
#include <stdio.h>
#include "robot/basicRobot.h"
#include "device/device.h"
#ifdef POLOLU
	#include "device/pololu.h"
#elif RASPI
	#include "device/raspi.h"
#elif PARALLAX
	#include "device/parallax.h"
#elif ARDUINO
	#include "device/arduino.h"
#else
	#include "device/dummyDevice.h"
#endif

//Constructors
BasicRobot::BasicRobot() {
	//Set up output device
	#ifdef POLOLU
		device = new Pololu();
	#elif RASPI
		device = new RasPi();
	#elif PARALLAX
		device = new Parallax();
	#elif ARDUINO
		device = new Arduino();
	#else
		device = new DummyDevice();
	#endif
}

BasicRobot::BasicRobot(const char *deviceFile) {
	printf("Output device file: %s\n", deviceFile);
	//Set up output device at specified filesystem location
	#ifdef POLOLU
		device = new Pololu(deviceFile);
	#elif RASPI
		device = new RasPi();
	#elif PARALLAX
		device = new Parallax(deviceFile);
	#elif ARDUINO
		device = new Arduino(deviceFile);
	#else
		device = new DummyDevice();
	#endif
}

//Destructor
BasicRobot::~BasicRobot() {
	delete device;
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	if(options != NULL) {
		return new BasicRobot(options);
	} else {
		return new BasicRobot();
	}
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Run motors
void BasicRobot::run(int numValues, double values[], int numButtons, bool buttons[]) {
	for(int i = 0; i < numValues; i++) {
		device->setMotorPower(i, values[i]);
	}

	//Buttons are currently ignored
}

//Stop motors
void BasicRobot::stop() {
	device->stopMotors();
}
