#ifndef RASPI_H
#define RASPI_H

//Includes
#include "motor.h"

//Declaration
class RasPi : private Motor {
public:
	//Constructor
	RasPi();

	//Destructor
	~RasPi();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Functions
	double scalePower(double power);
};

#endif //POLOLU_H
