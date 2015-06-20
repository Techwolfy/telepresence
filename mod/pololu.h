#ifndef POLOLU_H
#define POLOLU_H

//Includes
#include "motor.h"
#include "RPMSerialInterface.h"

//Declaration
class Pololu : private Motor {
public:
	//Constructor
	Pololu();

	Pololu(const char *file, int baud);

	//Destructor
	~Pololu();

	//Functions
	void control(double values[]);
	void stop();

private:
	//Variables
	RPM::SerialInterface *pololu;

	//Functions
	double scalePower(double power);
};

#endif //POLOLU_H