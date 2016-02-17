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
	virtual inline int getNumAnalogInputs() {return 0;}
	virtual inline double getAnalogInput(int analogNum) {unsupportedOperation(); return 0.0;}
	//Digital Input
	virtual inline int getNumDigitalInputs() {return 0;}
	virtual inline bool getDigitalInput(int digitalNum) {unsupportedOperation(); return false;}
	//Encoder Input
	virtual inline int getNumEncoders() {return 0;}
	virtual inline int getEncoderCount(int encoderNum) {unsupportedOperation(); return 0;}
	virtual inline double getEncoderRate(int encoderNum) {unsupportedOperation(); return 0.0;}
	//Digital Output
	virtual inline int getNumDigitalOutputs() {return 0;}
	virtual inline void setDigitalOutput(int outputNum, bool state) {unsupportedOperation();}
	virtual inline void clearDigitalOutputs() {unsupportedOperation();}
	//Motor Output
	virtual inline int getNumMotors() {return 0;}
	virtual inline void setMotorPower(int motorNum, double power) {unsupportedOperation();}
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
