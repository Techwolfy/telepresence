#ifndef ARDUINO_H
#define ARDUINO_H

//Includes
#include <termios.h>
#include "output/motor.h"

//Declaration
class Arduino : public Motor {
public:
	//Constructor
	Arduino();
	Arduino(const char *file);

	//Destructor
	~Arduino();

	//Functions
	void control(int numValues, double values[]);
	void stop();
	void setDigitalOutputs(int numValues, bool values[]);
	float getAnalogInput(unsigned char channel);
	long getEncoderCount(unsigned char channel);
	float getEncoderSpeed(unsigned char channel);

private:
	//Variables
	int arduinoFD;
	struct termios tty;

	//Functions
	unsigned short scalePower(double power);
	void setPower(unsigned char channel, unsigned short power);
	void setDigitalOutput(unsigned char channel, bool high);
};

#endif //ARDUINO_H
