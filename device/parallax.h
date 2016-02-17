#ifndef PARALLAX_H
#define PARALLAX_H

//Includes
#include <termios.h>
#include "device/device.h"

//Declaration
class Parallax : public Device {
public:
	//Constructor
	Parallax();
	Parallax(const char *file);

	//Destructor
	~Parallax();

	//Functions
	inline int getNumMotors() {return PARALLAX_NUM_MOTORS;}
	void setMotorPower(unsigned char channel, double power);
	void stopMotors();

private:
	//Constants
	const static int PARALLAX_NUM_MOTORS = 16;

	//Variables
	int parallaxFD;
	struct termios tty;

	//Functions
	void setBaudRate(bool increase);
	double getVersion();
	unsigned short scalePower(double power);
	unsigned short getPower(unsigned char channel);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //PARALLAX_H
