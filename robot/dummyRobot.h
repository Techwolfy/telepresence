#ifndef DUMMYROBOT_H
#define DUMMYROBOT_H

//Includes
#include "output/output.h"

//Declaration
class DummyRobot : public Output {
public:
	//Constructor
	DummyRobot();

	//Destructor
	~DummyRobot();

	//Functions
	void control(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();
};

#endif //DUMMYROBOT_H
