#ifndef POLOLU_H
#define POLOLU_H

//Includes
#include "device/device.h"
#include "util/serial.h"

//Declaration
class Pololu : public Device, private Serial {
public:
	//Constructor
	Pololu();
	Pololu(const char *file);

	//Destructor
	~Pololu();

	//Functions
	inline int getNumMotors() {return POLOLU_NUM_MOTORS;}
	void setMotorPower(unsigned char channel, double power);
	void stopMotors();

private:
	//Constants
	static const int POLOLU_NUM_MOTORS = 24;

	//Functions
	unsigned short scalePower(double power);
	unsigned short getPower(unsigned char channel);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //POLOLU_H
