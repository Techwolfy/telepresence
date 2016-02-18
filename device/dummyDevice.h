#ifndef DUMMYDEVICE_H
#define DUMMYDEVICE_H

//Includes
#include "device/device.h"
#include "util/ratelimit.h"

//Declaration
class DummyDevice : public Device {
public:
	//Constructor
	DummyDevice();

	//Destructor
	~DummyDevice();

	//Functions
	inline int getNumAnalogInputs() {return DUMMY_DEVICE_NUM_IO;};
	double getAnalogInput(int analogNum);
	//Digital Input
	inline int getNumDigitalInputs() {return DUMMY_DEVICE_NUM_IO;};
	bool getDigitalInput(int digitalNum);
	//Encoder Input
	inline int getNumEncoders() {return DUMMY_DEVICE_NUM_IO;};
	int getEncoderCount(int encoderNum);
	double getEncoderRate(int encoderNum);
	//Digital Output
	inline int getNumDigitalOutputs() {return DUMMY_DEVICE_NUM_IO;};
	void setDigitalOutput(int outputNum, bool state);
	void clearDigitalOutputs();
	//Motor Output
	inline int getNumMotors() {return DUMMY_DEVICE_NUM_IO;};
	void setMotorPower(int motorNum, double power);
	void stopMotors();

private:
	//Constants
	static const int DUMMY_DEVICE_NUM_IO = 10;

	//Variables
	Ratelimit messages;
	Ratelimit values;
	int gaiRateID, gaiValueRateID;
	int gdiRateID, gdiValueRateID;
	int gecRateID, gecValueRateID;
	int gerRateID, gerValueRateID;
	int sdoRateID, sdoValueRateID;
	int cdoRateID;
	int smpRateID, smpValueRateID;
	int smRateID;
};

#endif //DUMMYDEVICE_H
