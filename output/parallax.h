#ifndef PARALLAX_H
#define PARALLAX_H

//Includes
#include <termios.h>
#include "output/motor.h"

//Declaration
class Parallax : public Motor {
public:
	//Constructor
	Parallax();
	Parallax(const char *file, unsigned int baud);

	//Destructor
	~Parallax();

	//Functions
	void control(int numValues, double values[]);
	void stop();

private:
	//Variables
	int parallaxFD;
	struct termios tty;

	//Functions
	unsigned short scalePower(double power);
	void setPower(unsigned char servoNum, unsigned short power);
};

#endif //PARALLAX_H
