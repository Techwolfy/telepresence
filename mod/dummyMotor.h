#ifndef DUMMYMOTOR_H
#define DUMMYMOTOR_H

//Includes
#include "motor.h"

//Declaration
class DummyMotor : Motor {
public:
	//Constructor
	DummyMotor();

	//Destructor
	~DummyMotor();

	//Functions
	void control(double values[]);
	void stop();
};

#endif //DUMMYMOTOR_H