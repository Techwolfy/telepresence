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
	inline unsigned char getNumAnalogInputs() {return DUMMY_DEVICE_NUM_IO;};
	double getAnalogInput(unsigned char analogNum);
	//Digital Input
	inline unsigned char getNumDigitalInputs() {return DUMMY_DEVICE_NUM_IO;};
	bool getDigitalInput(unsigned char digitalNum);
	//Encoder Input
	inline unsigned char getNumEncoders() {return DUMMY_DEVICE_NUM_IO;};
	int getEncoderCount(unsigned char encoderNum);
	double getEncoderRate(unsigned char encoderNum);
	//Digital Output
	inline unsigned char getNumDigitalOutputs() {return DUMMY_DEVICE_NUM_IO;};
	void setDigitalOutput(unsigned char outputNum, bool state);
	void clearDigitalOutputs();
	//Motor Output
	inline unsigned char getNumMotors() {return DUMMY_DEVICE_NUM_IO;};
	void setMotorPower(unsigned char motorNum, double power);
	void stopMotors();

private:
	//Constants
	static const unsigned char DUMMY_DEVICE_NUM_IO = 10;

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
