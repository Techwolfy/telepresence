//BasicRobot.cpp

//Includes
#include "robot/basicRobot.h"
#include "output/motor.h"
#ifdef POLOLU
	#include "output/pololu.h"
#elif RASPI
	#include "output/raspi.h"
#elif PARALLAX
	#include "output/parallax.h"
#else
	#include "output/dummyMotor.h"
#endif

//Constructor
BasicRobot::BasicRobot() {
	//Set up motor
	#ifdef POLOLU
		motor = new Pololu();
	#elif RASPI
		motor = new RasPi();
	#elif PARALLAX
		motor = new Parallax();
	#else
		motor = new DummyMotor();
	#endif
}

//Destructor
BasicRobot::~BasicRobot() {
	delete motor;
}

//Shared library constructor
extern "C" Output* createRobot() {
	return new BasicRobot();
}

//Shared library destructor
extern "C" void destroyRobot(Output *output) {
	delete output;
}

//Functions
//Run motors
void BasicRobot::control(int numValues, double values[], int numButtons, bool buttons[]) {
	motor->control(numValues, values);
	//Buttons are currently ignored
}

//Stop motors
void BasicRobot::stop() {
	motor->stop();
}
