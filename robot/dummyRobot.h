#ifndef DUMMYROBOT_H
#define DUMMYROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "util/ratelimit.h"

//Declaration
class DummyRobot : public RobotInterface {
public:
	//Constructor
	DummyRobot();

	//Destructor
	~DummyRobot();

	//Functions
	void run(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	//Variables
	Ratelimit messages;
	int runRateID;
	int stopRateID;
};

#endif //DUMMYROBOT_H
