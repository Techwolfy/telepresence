#ifndef REDROBOT_H
#define REDROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "device/pololu.h"

//Declaration
class RedRobot : public RobotInterface {
public:
	//Constructor
	RedRobot();
	RedRobot(const char *pololuFile);

	//Destructor
	~RedRobot();

	//Functions
	void run(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	//Variables
	Pololu *device;
};

#endif //REDROBOT_H
