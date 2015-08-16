#ifndef BASICROBOT_H
#define BASICROBOT_H

//Includes
#include "output/output.h"
#include "output/motor.h"

//Declaration
class BasicRobot : public Output {
public:
	//Constructor
	BasicRobot();

	//Destructor
	~BasicRobot();

	//Functions
	void control(int numAxes, double axes[], int numButtons, bool buttons[]);
	void stop();

private:
	Motor *motor;
};

#endif //BASICROBOT_H
