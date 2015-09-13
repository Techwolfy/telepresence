#ifndef POLOLU_H
#define POLOLU_H

//Includes
#include "output/motor.h"
#include "lib/pololu/static/include/RPMSerialInterface.h"

//Declaration
class Pololu : public Motor {
public:
	//Constructor
	Pololu();
	Pololu(const char *file, unsigned int baud);

	//Destructor
	~Pololu();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Variables
	RPM::SerialInterface *pololu;

	//Functions
	unsigned short scalePower(double power);
};

#endif //POLOLU_H
