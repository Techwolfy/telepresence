#ifndef QUADCOPTERROBOT_H
#define QUADCOPTERROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "device/raspi.h"

//Declaration
class QuadcopterRobot : public RobotInterface {
public:
	//Constructor
	QuadcopterRobot();

	//Destructor
	~QuadcopterRobot();

	//Functions
	void run(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	//Variables
	RasPi *device;
	double throttle;
	double roll;
	double pitch;
	double yaw;
	double max;
	double fr;
	double fl;
	double br;
	double bl;

	//Functions
	double scaleQuad(double power);
	double convJoystick(double joy, double scale);
};

#endif //QUADCOPTERROBOT_H
