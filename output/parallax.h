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
	Parallax(const char *file);

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
	void setBaudRate(bool increase);
	double getVersion();
	unsigned short scalePower(double power);
	unsigned short getPower(unsigned char servoNum);
	void setPower(unsigned char servoNum, unsigned short power);
};

#endif //PARALLAX_H
