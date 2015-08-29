//RedRobot.cpp

//Includes
#include <stdio.h>
#include "robot/redRobot.h"
#include "output/motor.h"
#include "output/pololu.h"

//Constructor
RedRobot::RedRobot() {
	//Set up motor
	motor = new Pololu();
}

//Destructor
RedRobot::~RedRobot() {
	delete motor;
}

//Shared library constructor
extern "C" Output* createRobot() {
	return new RedRobot();
}

//Shared library destructor
extern "C" void destroyRobot(Output *output) {
	delete output;
}

//Functions
//Run motors
void RedRobot::control(int numValues, double values[], int numButtons, bool buttons[]) {
	if(!buttons[2]) {
		mappedValues[0] = 0.0;
		mappedValues[1] = (buttons[0] == true ? 0.75 : 0.0);
		mappedValues[2] = (buttons[0] == true ? -0.75 : 0.0);
		mappedValues[3] = values[0] + values[1];
		mappedValues[4] = values[0] - values[1];
		motor->control(5, mappedValues);
	} else {
		stop();
	}
}

//Stop motors
void RedRobot::stop() {
	motor->stop();
}
