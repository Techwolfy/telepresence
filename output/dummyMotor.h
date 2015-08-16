#ifndef DUMMYMOTOR_H
#define DUMMYMOTOR_H

//Includes
#include "output/motor.h"

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
};

#endif //DUMMYMOTOR_H
