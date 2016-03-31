#ifndef RASPI_H
#define RASPI_H

//Includes
#include "device/device.h"

//Declaration
class RasPi : public Device {
public:
	//Constructor
	RasPi();

	//Destructor
	~RasPi();

	//Functions
	inline unsigned char getNumMotors() {return RASPI_NUM_MOTORS;}
	void setMotorPower(unsigned char motorNum, double power);
	void stopMotors();

private:
	//Constants
	static const unsigned char RASPI_NUM_MOTORS = 8;

	//Variables
	int fd;

	//Functions
	unsigned int scalePower(double power);
	void setPower(unsigned int channel, unsigned int power);
};

#endif //RASPI_H
