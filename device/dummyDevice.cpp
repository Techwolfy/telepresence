//DummyDevice.cpp

//Includes
#include "device/dummyDevice.h"
#include "util/log.h"
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

	Log::logf(Log::INFO, "Dummy motor created.\n");
}

//Destructor
DummyDevice::~DummyDevice() {
	Log::logf(Log::INFO, "Dummy motor destroyed.\n");
}

//Functions
//Dummy analog input; output input number to console
double DummyDevice::getAnalogInput(unsigned char analogNum) {
	if(analogNum < 0 || analogNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy encoder!\n");
		return 0.0;
	}

	if(!messages.limitReached(gaiRateID)) {
		Log::logf(Log::DEBUG, "Dummy device getAnalogInput() called.\n");
		messages.increment(gaiRateID);
	}
	if(!values.limitReached(gaiValueRateID)) {
		Log::logf(Log::DEBUG, "Analog Input %d: %f\n", analogNum, 0.5);
		values.increment(gaiValueRateID);
	}
	return 0.5;
}

//Dummy digital input; output input number to console
bool DummyDevice::getDigitalInput(unsigned char digitalNum) {
	if(digitalNum < 0 || digitalNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy digital input!\n");
		return false;
	}

	if(!messages.limitReached(gdiRateID)) {
		Log::logf(Log::DEBUG, "Dummy device getDigitalInput() called.\n");
		messages.increment(gdiRateID);
	}
	if(!values.limitReached(gdiValueRateID)) {
		Log::logf(Log::DEBUG, "Digital Input %d: %c\n", digitalNum, 'T');
		values.increment(gdiValueRateID);
	}
	return true;
}

//Dummy encoder; output encoder number to console
int DummyDevice::getEncoderCount(unsigned char encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy encoder!\n");
		return 0;
	}

	if(!messages.limitReached(gecRateID)) {
		Log::logf(Log::DEBUG, "Dummy device getEncoderCount() called.\n");
		messages.increment(gecRateID);
		
	}
	if(!values.limitReached(gecValueRateID)) {
		Log::logf(Log::DEBUG, "Encoder %d Count: %d\n", encoderNum, 100);
		values.increment(gecValueRateID);
	}
	return 100;
}

//Dummy encoder; output encoder number to console
double DummyDevice::getEncoderRate(unsigned char encoderNum) {
	if(encoderNum < 0 || encoderNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy encoder!\n");
		return 0.0;
	}

	if(!messages.limitReached(gerRateID)) {
		Log::logf(Log::DEBUG, "Dummy device getEncoderRate() called.\n");
		messages.increment(gerRateID);
		
	}
	if(!values.limitReached(gerValueRateID)) {
		Log::logf(Log::DEBUG, "Encoder %d RateID: %f\n", encoderNum, 5.0);
		values.increment(gerValueRateID);
	}
	return 5.0;
}

//Dummy digital output; output target state to console
void DummyDevice::setDigitalOutput(unsigned char outputNum, bool state) {
	if(outputNum < 0 || outputNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy digital output!\n");
		return;
	}

	if(!messages.limitReached(sdoRateID)) {
		Log::logf(Log::DEBUG, "Dummy device setDigitalOutput() called.\n");
		messages.increment(sdoRateID);
		
	}
	if(!values.limitReached(sdoValueRateID)) {
		Log::logf(Log::DEBUG, "Digital Output %d: %c\n", outputNum, state ? 'T' : 'F');
		values.increment(sdoValueRateID);
	}
}

//Dummy digital output; output clear message to console
void DummyDevice::clearDigitalOutputs() {
	if(!messages.limitReached(cdoRateID)) {
		Log::logf(Log::DEBUG, "Dummy device clearDigitalOutputs() called.\n");
		messages.increment(cdoRateID);
	}
}

//Dummy motor; output target value to console
void DummyDevice::setMotorPower(unsigned char motorNum, double power) {
	if(motorNum < 0 || motorNum >= DUMMY_DEVICE_NUM_IO) {
		Log::logf(Log::WARN, "Invalid dummy motor!\n");
		return;
	}

	if(!messages.limitReached(smpRateID)) {
		Log::logf(Log::DEBUG, "Dummy device setMotorPower() called.\n");
		messages.increment(smpRateID);
		
	}
	if(!values.limitReached(smpValueRateID)) {
		Log::logf(Log::DEBUG, "Motor %d: %f\n", motorNum, power);
		values.increment(smpValueRateID);
	}
}

//Dummy motor; output stop message to console
void DummyDevice::stopMotors() {
	if(!messages.limitReached(smRateID)) {
		Log::logf(Log::DEBUG, "Dummy device stopMotors() called.\n");
		messages.increment(smRateID);
	}
}
