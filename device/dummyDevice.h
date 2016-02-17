#ifndef DUMMYDEVICE_H
#define DUMMYDEVICE_H

//Includes
#include "device/device.h"
#include "util/watchdog.h"

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
	const static int DUMMY_DEVICE_NUM_IO = 10;

	//Variables
	Watchdog ratelimitAI;
	Watchdog ratelimitDI;
	Watchdog ratelimitEC;
	Watchdog ratelimitER;
	Watchdog ratelimitDS;
	Watchdog ratelimitDC;
	Watchdog ratelimitMP;
	Watchdog ratelimitMS;
};

#endif //DUMMYDEVICE_H
