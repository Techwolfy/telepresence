#ifndef REDROBOT_H
#define REDROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "output/motor.h"

//Declaration
class RedRobot : public RobotInterface {
public:
	//Constructor
	RedRobot();
	RedRobot(const char *pololuFile);

	//Destructor
	~RedRobot();

	//Functions
	void control(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	Motor *motor;
	double mappedValues[5];
};

#endif //REDROBOT_H
