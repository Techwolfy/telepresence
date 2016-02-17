//DummyDevice.cpp

//Includes
#include <stdio.h>
#include "device/dummyDevice.h"
#include "util/watchdog.h"

//Constructor
DummyDevice::DummyDevice() : ratelimitAI(500),
							 ratelimitDI(500),
							 ratelimitEC(500),
							 ratelimitER(500),
							 ratelimitDS(500),
							 ratelimitDC(500),
							 ratelimitMP(500),
							 ratelimitMS(500) {
	printf("Dummy motor created.\n");
}

//Destructor
DummyDevice::~DummyDevice() {
	printf("Dummy motor destroyed.\n");
}

//Functions
//Dummy analog input; output input number to console
double DummyDevice::getAnalogInput(int analogNum) {
	if(analogNum < 0 || analogNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy encoder!\n");
		return 0.0;
	}

	if(ratelimitAI.expired()) {
		printf("Dummy device getAnalogInput() called.\n");
		printf("Analog Input %d: %f\n", analogNum, 0.5);
		ratelimitAI.reset();
	}
	return 0.5;
}

//Dummy digital input; output input number to console
bool DummyDevice::getDigitalInput(int digitalNum) {
	if(digitalNum < 0 || digitalNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy digital input!\n");
		return false;
	}

	if(ratelimitDI.expired()) {
		printf("Dummy device getDigitalInput() called.\n");
		printf("Digital Input %d: %c\n", digitalNum, 'T');
		ratelimitDI.reset();
	}
	return true;
}

//Dummy encoder; output encoder number to console
int DummyDevice::getEncoderCount(int encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy encoder!\n");
		return 0;
	}

	if(ratelimitEC.expired()) {
		printf("Dummy device getEncoderCount() called.\n");
		printf("Encoder %d Count: %d\n", encoderNum, 100);
		ratelimitEC.reset();
	}
	return 100;
}

//Dummy encoder; output encoder number to console
double DummyDevice::getEncoderRate(int encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy encoder!\n");
		return 0.0;
	}

	if(ratelimitER.expired()) {
		printf("Dummy device getEncoderRate() called.\n");
		printf("Encoder %d Rate: %f\n", encoderNum, 5.0);
		ratelimitER.reset();
	}
	return 5.0;
}

//Dummy digital output; output target state to console
void DummyDevice::setDigitalOutput(int outputNum, bool state) {
	if(outputNum < 0 || outputNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy digital output!\n");
		return;
	}

	if(ratelimitDS.expired()) {
		printf("Dummy device setDigitalOutput() called.\n");
		printf("Digital Output %d: %c\n", outputNum, state ? 'T' : 'F');
		ratelimitDS.reset();
	}
}

//Dummy digital output; output clear message to console
void DummyDevice::clearDigitalOutputs() {
	if(ratelimitDC.expired()) {
		printf("Dummy device clearDigitalOutputs() called.\n");
		ratelimitDC.reset();
	}
}

//Dummy motor; output target value to console
void DummyDevice::setMotorPower(int motorNum, double power) {
	if(motorNum < 0 || motorNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy motor!\n");
		return;
	}

	if(ratelimitMP.expired()) {
		printf("Dummy device setMotorPower() called.\n");
		printf("Motor %d: %f\n", motorNum, power);
		ratelimitMP.reset();
	}
}

//Dummy motor; output stop message to console
void DummyDevice::stopMotors() {
	if(ratelimitMS.expired()) {
		printf("Dummy device stopMotors() called.\n");
		ratelimitMS.reset();
	}
}
