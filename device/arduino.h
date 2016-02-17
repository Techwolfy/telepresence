#ifndef ARDUINO_H
#define ARDUINO_H

//Includes
#include <termios.h>
#include "device/device.h"

//Declaration
class Arduino : public Device {
public:
	//Constructor
	Arduino();
	Arduino(const char *file);

	//Destructor
	~Arduino();

	//Functions
	//Analog Input
	inline int getNumAnalogInputs() {return TELEDUINO_NUM_ANALOG_INPUTS;}
	double getAnalogInput(unsigned char channel);
	//Encoder Input
	inline int getNumEncoders() {return TELEDUINO_NUM_ENCODERS;}
	int getEncoderCount(unsigned char channel);
	double getEncoderSpeed(unsigned char channel);
	//Digital Output
	inline int getNumDigitalOutputs() {return TELEDUINO_NUM_DIGITAL_OUTPUTS;}
	void setDigitalOutput(unsigned char channel, bool state);
	void clearDigitalOutputs();
	//Motor Output
	inline int getNumMotors() {return TELEDUINO_NUM_MOTORS;}
	void setMotorPower(unsigned char channel, double power);
	void stopMotors();

private:
	//Constants
	const static int TELEDUINO_NUM_ANALOG_INPUTS = 6;
	const static int TELEDUINO_NUM_ENCODERS = 2;
	const static int TELEDUINO_NUM_DIGITAL_OUTPUTS = 3;
	const static int TELEDUINO_NUM_MOTORS = 5;

	//Variables
	int arduinoFD;
	struct termios tty;

	//Functions
	unsigned short scalePower(double power);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //ARDUINO_H
