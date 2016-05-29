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
	//FR: 3 (BCM27, Pin 13)
	//FL: 4 (BCM22, Pin 15)
	//BR: 5 (BCM23, Pin 16)
	//BL: 6 (BCM24, Pin 18)
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
	//Button map
	//0 1 2 -> | Enable |Forward |Throttle|
	//3 4 5 -> |  Left  | Hover  | Right  |
	//6 7 8 -> |SpinCCW |Backward| SpinCW |
	//  9   -> |++++++++|Joystick|++++++++|
	double buttonPowerAdjust = 0.1;

	//Start control values at 0 each frame
	throttle = 0.0;
	roll = 0.0;
	pitch = 0.0;
	yaw = 0.0;

	if(numButtons <= 0 || (numButtons >= 1 && !buttons[0])) {	//Quadcopter disabled
		throttle = 0.0;
		roll = 0.0;
		pitch = 0.0;
		yaw = 0.0;
	} else if(numButtons >= 10 && !buttons[9]) {	//Quadcopter enabled (button mode)
		//Velocity control buttons
		if(buttons[1]) {
			pitch += buttonPowerAdjust;
		}
		if(buttons[7]) {
			pitch += -buttonPowerAdjust;
		}
		if(buttons[3]) {
			roll += buttonPowerAdjust;
		}
		if(buttons[5]) {
			roll += -buttonPowerAdjust;
		}
		if(buttons[6]) {
			yaw += buttonPowerAdjust;
		}
		if(buttons[8]) {
			yaw += -buttonPowerAdjust;
		}

		//Throttle control buttons
		if(buttons[2]) {
			throttle += buttonPowerAdjust;
		}
		if(buttons[4]) {
			throttle += buttonPowerAdjust;
		}
	} else if(buttons[9] && numValues >= 3) {	//Quadcopter enabled (joystick mode)
		//Retreive joystick values for flight control
		//FIXME: Temporarily disabled!
		//throttle = convJoystick(values[1], 0.5);	//Joystick 0,Y (Up/Down)
		//pitch = convJoystick(values[3], 0.5);		//Joystick 1,Y (Front/Back)
		//roll = convJoystick(values[2], 0.5);		//Joystick 1,X (Left/Right)
		//yaw = convJoystick(values[0], 0.5);			//Joystick 0,X (Rotate Clockwise/Counterclockwise)
	} else {	//Invalid case, disable motors
		throttle = 0.0;
		roll = 0.0;
		pitch = 0.0;
		yaw = 0.0;
	}

	//Calculate speeds for each motor
	fr = throttle +pitch +yaw;
	fl = throttle +roll -yaw;
	br = throttle -pitch +yaw;
	bl = throttle -roll -yaw;
	Log::logf(Log::DEBUG, "Throttle: %f, Yaw: %f, Roll: %f, Pitch: %f", throttle, yaw, roll, pitch);
	Log::logf(Log::DEBUG, "FR: %f, FL: %f, BR: %f, BL: %f", fr, fl, br, bl);

	//Run motors
	device->setMotorPower(3, scaleQuad(fr));
	device->setMotorPower(4, scaleQuad(fl));
	device->setMotorPower(5, scaleQuad(br));
	device->setMotorPower(6, scaleQuad(bl));
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

//Convert input from joystick to quadcopter ESC input range
double QuadcopterRobot::convJoystick(double joy, double scale) {
	//[-1.0, 1.0] to [0.0, 1.0]
	joy += 1.0;
	joy /= 2.0;

	//Scale value to limit range (e.g. 0.5 -> [0.0, 0.5])
	joy *= scale;
	return joy;
}
