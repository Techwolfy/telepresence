//DummyDevice.cpp

//Includes
#include <stdio.h>
#include "device/dummyDevice.h"
#include "util/ratelimit.h"

//Constructor
DummyDevice::DummyDevice() : messages(500, 1),
							 values(500, DUMMY_DEVICE_NUM_IO) {
	gaiRateID = messages.getID();
	gdiRateID = messages.getID();
	gecRateID = messages.getID();
	gerRateID = messages.getID();
	sdoRateID = messages.getID();
	cdoRateID = messages.getID();
	smpRateID = messages.getID();
	smRateID = messages.getID();
	gaiValueRateID = values.getID();
	gdiValueRateID = values.getID();
	gecValueRateID = values.getID();
	gerValueRateID = values.getID();
	sdoValueRateID = values.getID();
	smpValueRateID = values.getID();

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

	if(!messages.limitReached(gaiRateID)) {
		printf("Dummy device getAnalogInput() called.\n");
		messages.increment(gaiRateID);
	}
	if(!values.limitReached(gaiValueRateID)) {
		printf("Analog Input %d: %f\n", analogNum, 0.5);
		values.increment(gaiValueRateID);
	}
	return 0.5;
}

//Dummy digital input; output input number to console
bool DummyDevice::getDigitalInput(int digitalNum) {
	if(digitalNum < 0 || digitalNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy digital input!\n");
		return false;
	}

	if(!messages.limitReached(gdiRateID)) {
		printf("Dummy device getDigitalInput() called.\n");
		messages.increment(gdiRateID);
	}
	if(!values.limitReached(gdiValueRateID)) {
		printf("Digital Input %d: %c\n", digitalNum, 'T');
		values.increment(gdiValueRateID);
	}
	return true;
}

//Dummy encoder; output encoder number to console
int DummyDevice::getEncoderCount(int encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy encoder!\n");
		return 0;
	}

	if(!messages.limitReached(gecRateID)) {
		printf("Dummy device getEncoderCount() called.\n");
		messages.increment(gecRateID);
		
	}
	if(!values.limitReached(gecValueRateID)) {
		printf("Encoder %d Count: %d\n", encoderNum, 100);
		values.increment(gecValueRateID);
	}
	return 100;
}

//Dummy encoder; output encoder number to console
double DummyDevice::getEncoderRate(int encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy encoder!\n");
		return 0.0;
	}

	if(!messages.limitReached(gerRateID)) {
		printf("Dummy device getEncoderRate() called.\n");
		messages.increment(gerRateID);
		
	}
	if(!values.limitReached(gerValueRateID)) {
		printf("Encoder %d RateID: %f\n", encoderNum, 5.0);
		values.increment(gerValueRateID);
	}
	return 5.0;
}

//Dummy digital output; output target state to console
void DummyDevice::setDigitalOutput(int outputNum, bool state) {
	if(outputNum < 0 || outputNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy digital output!\n");
		return;
	}

	if(!messages.limitReached(sdoRateID)) {
		printf("Dummy device setDigitalOutput() called.\n");
		messages.increment(sdoRateID);
		
	}
	if(!values.limitReached(sdoValueRateID)) {
		printf("Digital Output %d: %c\n", outputNum, state ? 'T' : 'F');
		values.increment(sdoValueRateID);
	}
}

//Dummy digital output; output clear message to console
void DummyDevice::clearDigitalOutputs() {
	if(!messages.limitReached(cdoRateID)) {
		printf("Dummy device clearDigitalOutputs() called.\n");
		messages.increment(cdoRateID);
	}
}

//Dummy motor; output target value to console
void DummyDevice::setMotorPower(int motorNum, double power) {
	if(motorNum < 0 || motorNum >= DUMMY_DEVICE_NUM_IO) {
		printf("Invalid dummy motor!\n");
		return;
	}

	if(!messages.limitReached(smpRateID)) {
		printf("Dummy device setMotorPower() called.\n");
		messages.increment(smpRateID);
		
	}
	if(!values.limitReached(smpValueRateID)) {
		printf("Motor %d: %f\n", motorNum, power);
		values.increment(smpValueRateID);
	}
}

//Dummy motor; output stop message to console
void DummyDevice::stopMotors() {
	if(!messages.limitReached(smRateID)) {
		printf("Dummy device stopMotors() called.\n");
		messages.increment(smRateID);
	}
}
