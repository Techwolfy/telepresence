//QuadcopterRobot.cpp

//Includes
#include <unistd.h>
#include "robot/quadcopterRobot.h"
#include "device/raspi.h"

//Constructor
QuadcopterRobot::QuadcopterRobot() : throttle(0.0),
									 roll(0.0),
									 pitch(0.0),
									 yaw(0.0),
									 max(0.0),
									 fr(0.0),
									 fl(0.0),
									 br(0.0),
									 bl(0.0) {
	//Set up output device
	device = new RasPi();

	//Enable ESCs
	for(int i = 3; i <= 6; i++) {
		device->setMotorPower(i, scaleQuad(1.0));
	}
	usleep(1000);
	for(int i = 3; i <= 6; i++) {
		device->setMotorPower(i, scaleQuad(0.0));
	}
	usleep(1000);

	//Ensure all motors are stopped
	stop();
}

//Destructor
QuadcopterRobot::~QuadcopterRobot() {
	delete device;
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	return new QuadcopterRobot();
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Run motors
void QuadcopterRobot::run(int numValues, double values[], int numButtons, bool buttons[]) {
	//Retreive joystick values for flight calculations
	if(numValues >= 3) {
		throttle = values[1];	//Joystick 0,Y (Up/Down)
		pitch = values[3];		//Joystick 1,Y (Front/Back)
		roll = values[2];		//Joystick 1,X (Left/Right)
		yaw = values[0];		//Joystick 0,X (Rotate Clockwise/Counterclockwise)
	}

	if(throttle < 0.0) {
		throttle = 0.0;
	} else {
		throttle *= 0.5;
	}

	//Optionally disable rotation (allows easier throttle control)
	if(numButtons >= 3 && buttons[2]) {
		yaw = 0.0;
	}

	//Calculate speeds for each motor
	max = abs(pitch) + abs(roll) + abs(yaw);
	fr = (2/3)*throttle + (1/(3*max))*(+pitch +roll -yaw);
	fl = (2/3)*throttle + (1/(3*max))*(+pitch -roll +yaw);
	br = (2/3)*throttle + (1/(3*max))*(-pitch +roll +yaw);
	bl = (2/3)*throttle + (1/(3*max))*(-pitch -roll -yaw);
	Log::logf(Log::DEBUG, "Throttle: %f, Yaw: %f, Roll: %f, Pitch: %f", throttle, yaw, roll, pitch);
	Log::logf(Log::DEBUG, "FR: %f, FL: %f, BR: %f, BL: %f", fr, fl, br, bl);

	if(numButtons <= 0 || (numButtons >= 1 && !buttons[0])) {	//Quadcopter disabled
		stop();
	} else if(numButtons >= 2 && buttons[1]) {	//Quadcopter enabled (test mode)
		//Set all propellers to 20% power
		device->setMotorPower(3, scaleQuad(0.2));
		device->setMotorPower(4, scaleQuad(0.2));
		device->setMotorPower(5, scaleQuad(0.2));
		device->setMotorPower(6, scaleQuad(0.2));
	} else {									//Quadcopter enabled (flight mode)
		//device->setMotorPower(3, fr);
		//device->setMotorPower(4, fl);
		//device->setMotorPower(5, br);
		//device->setMotorPower(6, bl);
	}
}

//Stop motors
void QuadcopterRobot::stop() {
	for(int i = 3; i <= 6; i++) {
		device->setMotorPower(i, scaleQuad(0.0));
	}
}

//Scale power from quadcopter ESC range of (0.0, 1.0) to output range of (-1.0, 1.0)
double QuadcopterRobot::scaleQuad(double power) {
	if(power < 0.0) {
		power = 0.0;
	} else if(power > 1.0) {
		power = 1.0;
	}

	return (power * 2.0) - 1.0;
}
