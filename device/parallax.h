#ifndef PARALLAX_H
#define PARALLAX_H

//Includes
#include "device/device.h"
#include "util/serial.h"

//Declaration
class Parallax : public Device, private Serial {
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
	static const int PARALLAX_NUM_MOTORS = 16;

	//Functions
	using Serial::setBaudRate;
	void setBaudRate(bool increase);
	double getVersion();
	unsigned short scalePower(double power);
	unsigned short getPower(unsigned char channel);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //PARALLAX_H
