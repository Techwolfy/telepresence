//Pololu.cpp

//Includes
#include <stdio.h>
#include <termios.h>
#include <stdexcept>
#include "device/pololu.h"

//Constructor
Pololu::Pololu() : Pololu("/dev/ttyACM0") {

}

Pololu::Pololu(const char *file) : Serial(file, 115200, false, false, 2, 0) {
	//Set up Pololu Maestro servo controller interface
	//Serial connection is 115200 8 N 1, full duplex
		//https://www.pololu.com/docs/0J40/5.b

	printf("Pololu Maestro servo controller initialized!\n");

	//Make sure all motors are stopped
	stopMotors();
}

//Destructor
Pololu::~Pololu() {
	stopMotors();
}

//Functions
//Set power of a specific motor
//NB: Supports up to 24-port Pololu Maestro servo controllers; however, smaller controllers will ignore commands for ports they don't have.
void Pololu::setMotorPower(unsigned char channel, double power) {
	setPower(channel, scalePower(power));
}

//Stop all motors
void Pololu::stopMotors() {
	for(int i = 0; i < POLOLU_NUM_MOTORS; i++) {
		setPower(i, scalePower(0.0));
	}
}

//Scale power from (-1.0, 1.0) to Pololu range of (4000, 8000)
unsigned short Pololu::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (4000, 8000) quarter-of-microseconds; 0 is 6000 (neutral)
	return (power * 2000) + 6000;
}

//Get power of a specific channel
unsigned short Pololu::getPower(unsigned char channel) {
	if(channel > POLOLU_NUM_MOTORS) {
		printf("Invalid PWM channel!\n");
		return 0;
	}

	unsigned char command[2] = {0x90, channel};
	//Command byte, channel byte
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Pololu Maestro servo controller!\n");
	}

	unsigned char data[2] = {0};
	if(serialRead(&data, sizeof(data)) == sizeof(data)) {
		return (data[0] << 8) | data[1];
	} else {
		printf("Error reading from Pololu Maestro servo controller!\n");
		return 0;
	}
}

//Set power of a specific channel
void Pololu::setPower(unsigned char channel, unsigned short power) {
	if(channel > POLOLU_NUM_MOTORS) {
		printf("Invalid PWM channel!\n");
		return;
	}

	unsigned char command[4] = {0x84, channel, (unsigned char)(power & 0x7F), (unsigned char)((power >> 7) & 0x7F)};
	//Command byte, channel byte, target low 7 bits, target high 7 bits
	if(serialWrite(&command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Pololu Maestro servo controller!\n");
	}
}
