#ifndef DUMMYMOTOR_H
#define DUMMYMOTOR_H

//Includes
#include "output/motor.h"
#include "util/watchdog.h"

//Declaration
class DummyMotor : public Motor {
public:
	//Constructor
	DummyMotor();

	//Destructor
	~DummyMotor();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Variables
	Watchdog ratelimitControl;
	Watchdog ratelimitStop;
};

#endif //DUMMYMOTOR_H
