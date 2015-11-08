#ifndef POLOLU_H
#define POLOLU_H

//Includes
#include "output/motor.h"

//Declaration
class Pololu : public Motor {
public:
	//Constructor
	Pololu();
	Pololu(const char *file);

	//Destructor
	~Pololu();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Variables
	int pololuFD;

	//Functions
	unsigned short scalePower(double power);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //POLOLU_H
