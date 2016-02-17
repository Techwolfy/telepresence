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
	inline int getNumMotors() {return RASPI_NUM_MOTORS;}
	void setMotorPower(int motorNum, double power);
	void stopMotors();

private:
	//Constants
	const static int RASPI_NUM_MOTORS = 10;

	//Functions
	double scalePower(double power);
};

#endif //RASPI_H
