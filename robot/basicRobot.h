#ifndef BASICROBOT_H
#define BASICROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "device/device.h"

//Declaration
class BasicRobot : public RobotInterface {
public:
	//Constructor
	BasicRobot();
	BasicRobot(const char *deviceFile);

	//Destructor
	~BasicRobot();

	//Functions
	void run(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	Device *device;
};

#endif //BASICROBOT_H
