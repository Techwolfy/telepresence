#ifndef DUMMYROBOT_H
#define DUMMYROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "util/watchdog.h"

//Declaration
class DummyRobot : public RobotInterface {
public:
	//Constructor
	DummyRobot();

	//Destructor
	~DummyRobot();

	//Functions
	void control(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	//Variables
	Watchdog ratelimitControl;
	Watchdog ratelimitStop;
};

#endif //DUMMYROBOT_H
