#ifndef POLOLU_H
#define POLOLU_H

//Includes
#include <termios.h>
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
	struct termios tty;

	//Functions
	unsigned short scalePower(double power);
	unsigned short getPower(unsigned char channel);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //POLOLU_H
