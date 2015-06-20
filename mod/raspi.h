#ifndef RASPI_H
#define RASPI_H

//Includes
#include "motor.h"

//Declaration
class RasPi : Motor {
public:
	//Constructor
	RasPi();

	//Destructor
	~RasPi();

	//Functions
	void control(double values[]);
	void stop();

private:
	//Functions
	double scalePower(double power);
};

#endif //POLOLU_H