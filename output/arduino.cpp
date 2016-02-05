//Arduino.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdexcept>
#include "output/arduino.h"

//Constructor
Arduino::Arduino() : Arduino("/dev/ttyACM0") {

}

Arduino::Arduino(const char *file) {
	//Set up Arduino serial interface
	arduinoFD = open(file, O_RDWR | O_NOCTTY);
	if(arduinoFD < 0) {
		printf("Arduino initialization failed!\n");
		throw std::runtime_error("arduino initialization failed");
	}

	//Serial connection is 115200 8 N 1, full duplex
		//https://www.arduino.cc/en/Serial/Begin

	//Input/output flags
	tty.c_iflag = 0;		//No input processing
	tty.c_oflag = 0;		//No output processing
	//Control flags
	tty.c_cflag = CSIZE;	//Character size mask
	tty.c_cflag |= CS8;		//8 data bits
	tty.c_cflag &= ~PARENB;	//No parity
	tty.c_cflag &= ~CSTOPB;	//1 stop bit
	tty.c_cflag |= CREAD;	//Enable read
	tty.c_cflag |= CLOCAL;	//Ignore control lines
	//Local flags
	tty.c_lflag = 0;		//No local processing: use non-canonical (raw) mode, disable echo, etc.
	//Control characters
	tty.c_cc[VMIN] = 2;		//Block on read until 2 chars have been read
	tty.c_cc[VTIME] = 0;	//Read timeout not used

	//Set intial baud rate
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	//Flush serial port and apply settings
	if(tcsetattr(arduinoFD, TCSAFLUSH, &tty) != 0) {
		perror("Error applying Arduino serial port settings");
		throw std::runtime_error("arduino initialization failed");
	}

	printf("Arduino initialized!\n");

	//Make sure all motors are stopped
	stop();
}

//Destructor
Arduino::~Arduino() {
	stop();
	close(arduinoFD);
}

//Functions
//NB: Ports and values outside the range of the connected Arduino's capabilities will be ignored
//Set power of all motors
void Arduino::control(int numValues, double values[]) {
	for(int i = 0; i < numValues && i < 6; i++) {
		setPower(i, scalePower(values[i]));
	}
}

//Stop all motors
void Arduino::stop() {
	for(int i = 0; i < 6; i++) {
		setPower(i, scalePower(0));
	}
}

//Set digital output values
void Arduino::setDigitalOutputs(int numValues, bool values[]) {
	for(int i = 0; i < numValues && i < 6; i++) {
		setDigitalOutput(i, values[i]);
	}
}

//Retrieve the value of an analog input channel
float Arduino::getAnalogInput(unsigned char channel) {
	if(channel > 6) {
		return 0.0;
	}

	unsigned char command[4] = {'A', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(write(arduinoFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Arduino!\n");
	}

	float data = 0.0f;
	//Response is a 32-bit IEEE 754 float
	if(read(arduinoFD, &data, sizeof(data)) != sizeof(data)) {
		printf("Error reading from Arduino!\n");
	}

	return data;
}

//Retieve the count of a rotary encoder connected to the specified channel
//NB: Speeds faster than the Arduino's capability to measure may be inaccurate
long Arduino::getEncoderCount(unsigned char channel) {
	if(channel > 4) {
		return 0;
	}

	unsigned char command[4] = {'C', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(write(arduinoFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Arduino!\n");
	}

	unsigned char data[4] = {0};
	//Response is a 4-byte little-endian signed integer (translation may be redundant)
	if(read(arduinoFD, &data, sizeof(data)) != sizeof(data)) {
		printf("Error reading from Arduino!\n");
	}
	return (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
}

//Retieve the speed of a rotary encoder connected to the specified channel (counts/sec)
//NB: Speeds faster than the Arduino's capability to measure may be inaccurate
float Arduino::getEncoderSpeed(unsigned char channel) {
	if(channel > 4) {
		return 0.0;
	}

	unsigned char command[4] = {'E', channel, 0x00, 0x00};
	//Command byte, channel byte, 2 null bytes
	if(write(arduinoFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Arduino!\n");
	}

	float data = 0.0f;
	//Response is a 32-bit IEEE 754 float
	if(read(arduinoFD, &data, sizeof(data)) != sizeof(data)) {
		printf("Error reading from Arduino!\n");
	}
	return data;
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

//Set power of a specific channel
void Arduino::setPower(unsigned char channel, unsigned short power) {
	if(channel > 6) {
		printf("Invalid PWM channel!\n");
		return;
	}

	unsigned char command[4] = {'P', channel, (unsigned char)(power & 0xFF), (unsigned char)((power >> 8) & 0xFF)};
	//Command byte, channel byte, target low byte, target high byte
	if(write(arduinoFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Arduino!\n");
	}
}

//Set state of a specific digital I/O port
void Arduino::setDigitalOutput(unsigned char channel, bool high) {
	if(channel > 4) {
		printf("Invalid Digital I/O channel!\n");
		return;
	}

	unsigned char command[4] = {'D', channel, high, '\0'};
	//Command byte, channel byte, state byte, null byte
	if(write(arduinoFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Arduino!\n");
	}
}
