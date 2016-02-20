#ifndef ARDUINO_H
#define ARDUINO_H

//Includes
#include "device/device.h"
#include "util/serial.h"

//Declaration
class Arduino : public Device, private Serial {
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
	static const int TELEDUINO_NUM_ANALOG_INPUTS = 6;
	static const int TELEDUINO_NUM_ENCODERS = 2;
	static const int TELEDUINO_NUM_DIGITAL_OUTPUTS = 3;
	static const int TELEDUINO_NUM_MOTORS = 5;

	//Functions
	unsigned short scalePower(double power);
	void setPower(unsigned char channel, unsigned short power);
};

#endif //ARDUINO_H
