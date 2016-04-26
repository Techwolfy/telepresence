//Arduino.cpp

//Includes
#include <stdexcept>
#include "device/arduino.h"
#include "util/log.h"

//Constructor
Arduino::Arduino() : Arduino("/dev/ttyACM0") {

}

Arduino::Arduino(const char *file) : Serial(file, 115200, false, false, 2, 0) {
	//Set up Arduino serial interface
	//Serial connection is 115200 8 N 1, full duplex
		//https://www.arduino.cc/en/Serial/Begin

	Log::logf(Log::INFO, "Arduino initialized!\n");

	//Make sure all motors are stopped
	stopMotors();
}

//Destructor
Arduino::~Arduino() {
	stopMotors();
}

//Functions
//NB: Ports and values outside the range of the connected Arduino's capabilities will be ignored
//Retrieve the value of an analog input channel
double Arduino::getAnalogInput(unsigned char channel) {
	if(channel > TELEDUINO_NUM_ANALOG_INPUTS) {
		Log::logf(Log::WARN, "Invalid analog input channel!\n");
		return 0.0;
	}

	unsigned char command[4] = {'A', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		Log::logf(Log::ERR, "Error writing to Arduino!\n");
	}

	float data = 0.0f;
	//Response is a 32-bit IEEE 754 float
	if(serialRead(&data, sizeof(data)) != sizeof(data)) {
		Log::logf(Log::ERR, "Error reading from Arduino!\n");
	}

	return data;	//Float is promoted to double
}

//Retieve the count of a rotary encoder connected to the specified channel
//NB: Speeds faster than the Arduino's capability to measure may be inaccurate
int Arduino::getEncoderCount(unsigned char channel) {
	if(channel > TELEDUINO_NUM_ENCODERS) {
		Log::logf(Log::WARN, "Invalid encoder channel!\n");
		return 0;
	}

	unsigned char command[4] = {'C', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		Log::logf(Log::ERR, "Error writing to Arduino!\n");
	}

	unsigned char data[4] = {0};
	//Response is a 4-byte little-endian signed integer (translation may be redundant)
	if(serialRead(&data, sizeof(data)) != sizeof(data)) {
		Log::logf(Log::ERR, "Error reading from Arduino!\n");
	}
	return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
}

//Retieve the speed of a rotary encoder connected to the specified channel (counts/sec)
//NB: Speeds faster than the Arduino's capability to measure may be inaccurate
double Arduino::getEncoderSpeed(unsigned char channel) {
	if(channel > TELEDUINO_NUM_ENCODERS) {
		Log::logf(Log::WARN, "Invalid encoder channel!\n");
		return 0.0;
	}

	unsigned char command[4] = {'E', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		Log::logf(Log::ERR, "Error writing to Arduino!\n");
	}

	float data = 0.0f;
	//Response is a 32-bit IEEE 754 float
	if(serialRead(&data, sizeof(data)) != sizeof(data)) {
		Log::logf(Log::ERR, "Error reading from Arduino!\n");
	}
	return data;
}

//Set state of a specific digital I/O port
void Arduino::setDigitalOutput(unsigned char channel, bool state) {
	if(channel > TELEDUINO_NUM_DIGITAL_OUTPUTS) {
		Log::logf(Log::WARN, "Invalid digital output channel!\n");
		return;
	}

	unsigned char command[4] = {'D', channel, state, '\0'};
	//Command byte, channel byte, state byte, null byte
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		Log::logf(Log::ERR, "Error writing to Arduino!\n");
	}
}

//Clear all digital I/O ports
void Arduino::clearDigitalOutputs() {
	for(unsigned char i = 0; i < TELEDUINO_NUM_DIGITAL_OUTPUTS; i++) {
		setDigitalOutput(i, false);
	}
}

//Set power of a specific motor
void Arduino::setMotorPower(unsigned char channel, double power) {
	setPower(channel, scalePower(power));
}

//Stop all motors
void Arduino::stopMotors() {
	for(int i = 0; i < TELEDUINO_NUM_MOTORS; i++) {
		setPower(i, scalePower(0.0));
	}
}

//Scale power from (-1.0, 1.0) to Arduino range of (1000, 2000)
unsigned short Arduino::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (1000, 2000) microseconds; 0 is 1500 (neutral)
	return (power * 500) + 1500;
}

//Set power of a specific motor channel
void Arduino::setPower(unsigned char channel, unsigned short power) {
	if(channel > TELEDUINO_NUM_MOTORS) {
		Log::logf(Log::WARN, "Invalid motor channel!\n");
	}

	unsigned char command[4] = {'P', channel, (unsigned char)(power & 0xFF), (unsigned char)((power >> 8) & 0xFF)};
	//Command byte, channel byte, target low byte, target high byte
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		Log::logf(Log::ERR, "Error writing to Arduino!\n");
	}
}
