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
	Pololu(const char *file, int baud);

	//Destructor
	~Pololu();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Variables
	RPM::SerialInterface *pololu;

	//Functions
	double scalePower(double power);
};

#endif //POLOLU_H
