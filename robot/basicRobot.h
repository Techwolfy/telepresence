#ifndef BASICROBOT_H
#define BASICROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "output/motor.h"

//Declaration
class BasicRobot : public RobotInterface {
public:
	//Constructor
	BasicRobot();

	BasicRobot(const char *deviceFile);

	//Destructor
	~BasicRobot();

	//Functions
	void control(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	Motor *motor;
};

#endif //BASICROBOT_H
