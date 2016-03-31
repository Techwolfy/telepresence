#ifndef DEVICE_H
#define DEVICE_H

//Includes
#include <stdio.h>
#include <stdexcept>

//Declaration
class Device {
public:
	//Destructor
	virtual ~Device() = default;

	//Functions
	//Analog Input
	virtual inline unsigned char getNumAnalogInputs() {return 0;}
	virtual inline double getAnalogInput(unsigned char analogNum) {unsupportedOperation(); return 0.0;}
	//Digital Input
	virtual inline unsigned char getNumDigitalInputs() {return 0;}
	virtual inline bool getDigitalInput(unsigned char digitalNum) {unsupportedOperation(); return false;}
	//Encoder Input
	virtual inline unsigned char getNumEncoders() {return 0;}
	virtual inline int getEncoderCount(unsigned char encoderNum) {unsupportedOperation(); return 0;}
	virtual inline double getEncoderRate(unsigned char encoderNum) {unsupportedOperation(); return 0.0;}
	//Digital Output
	virtual inline unsigned char getNumDigitalOutputs() {return 0;}
	virtual inline void setDigitalOutput(unsigned char outputNum, bool state) {unsupportedOperation();}
	virtual inline void clearDigitalOutputs() {unsupportedOperation();}
	//Motor Output
	virtual inline unsigned char getNumMotors() {return 0;}
	virtual inline void setMotorPower(unsigned char motorNum, double power) {unsupportedOperation();}
	virtual inline void stopMotors() {unsupportedOperation();}

protected:
	//Constructor ("protected" idenfitier prevents Device class from being instantiated)
	Device() = default;

private:
	inline void unsupportedOperation() {
		printf("Unsupported device operation called!\n");
		throw std::runtime_error("unsupported device operation called");
	}
};

#endif //DEVICE_H
