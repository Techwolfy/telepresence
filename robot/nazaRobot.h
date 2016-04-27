#ifndef NAZAROBOT_H
#define NAZAROBOT_H

//Includes
#include "robot/robotInterface.h"
#include "device/raspi.h"

//Declaration
class NazaRobot : public RobotInterface {
public:
	//Constructor
	NazaRobot();

	//Destructor
	~NazaRobot();

	//Functions
	void run(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	//Variables
	RasPi *device;
};

#endif //NAZAROBOT_H
