//RedRobot.cpp

//Includes
#include "robot/redRobot.h"
#include "device/device.h"
#include "device/pololu.h"

//Constructor
RedRobot::RedRobot() {
	//Set up output device
	device = new Pololu();
}

RedRobot::RedRobot(const char *pololuFile) {
	//Set up output device
	device = new Pololu(pololuFile);
}

//Destructor
RedRobot::~RedRobot() {
	delete device;
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	return new RedRobot();
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Run motors
void RedRobot::run(int numValues, double values[], int numButtons, bool buttons[]) {
	if(!buttons[2]) {
		device->setMotorPower(0, 0.0);
		device->setMotorPower(1, (buttons[0] == true ? 0.75 : 0.0));
		device->setMotorPower(2, (buttons[0] == true ? -0.75 : 0.0));
		device->setMotorPower(3, values[0] + values[1]);
		device->setMotorPower(4, values[0] - values[1]);
	} else {
		device->stopMotors();
	}
}

//Stop motors
void RedRobot::stop() {
	device->stopMotors();
}
