//NazaRobot.cpp

//Includes
#include <unistd.h>
#include "robot/nazaRobot.h"
#include "device/raspi.h"

//Constructor
NazaRobot::NazaRobot() {
	//Set up output device
	device = new RasPi();

	//Enable NAZA-M Lite controller
	device->setMotorPower(3, -1.0);
	device->setMotorPower(4, -1.0);
	device->setMotorPower(5, -1.0);
	device->setMotorPower(6, 1.0);
	device->setMotorPower(7, 0.0);
	usleep(1000);

	//Ensure all motors are stopped
	device->stopMotors();
}

//Destructor
NazaRobot::~NazaRobot() {
	delete device;
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	return new NazaRobot();
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Run motors
void NazaRobot::run(int numValues, double values[], int numButtons, bool buttons[]) {
	//Set NAZA-M Lite flight inputs directly
	if(numButtons >= 1 && !buttons[0]) {		//Quadcopter disabled
		device->stopMotors();
	} else {									//Quadcopter enabled
		if(numValues >= 4) {
			device->setMotorPower(3, values[2]);	//Aileron (Roll, Left/Right)
			device->setMotorPower(4, values[3]);	//Elevator (Pitch, Front/Back)
			device->setMotorPower(5, values[1]);	//Throttle (Up/Down)
			device->setMotorPower(6, values[0]);	//Rudder (Yaw, Rotate Clockwise/Counterclockwise)
			device->setMotorPower(7, 0.0);			//U (Control Mode, Manual/Assisted)
		}
	}
}

//Stop motors
void NazaRobot::stop() {
	device->stopMotors();
}
