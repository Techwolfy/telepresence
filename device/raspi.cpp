//RasPi.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include "device/raspi.h"

//Constructor
RasPi::RasPi() : fd(0) {
	//Set up Pi-Blaster
	fd = open("/dev/pi-blaster", O_WRONLY);
	if(fd < 0) {
		printf("Error opening ServoBlaster!\n");
		throw std::runtime_error("raspi servoblaster initialization failed");
	}

	//Make sure all motors are stopped
	stopMotors();

	printf("ServoBlaster GPIO Software PWM initialized!\n");
}

//Destructor
RasPi::~RasPi() {
	stopMotors();
	close(fd);
}

//Functions
//Set power of a specific motor
void RasPi::setMotorPower(unsigned int motorNum, double power) {
	setPower(motorNum, scalePower(power));
}

//Stop all motors
void RasPi::stopMotors() {
	for(int i = 0; i < RASPI_NUM_MOTORS; i++) {
		setPower(i, scalePower(0.0));
	}
}

//Scale power from (-1.0, 1.0) to RasPi range of (100, 200)
unsigned int RasPi::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (100, 200) microseconds * 10; 0 is 150 (neutral)
	return (power * 50) + 150;
}

void RasPi::setPower(unsigned int channel, unsigned int power) {
	if(channel > RASPI_NUM_MOTORS) {
		printf("Invalid motor channel!\n");
	}

	//1.0ms full reverse, 1.5ms neural, 2.0ms full forward
	char command[7] = {0};
	sprintf(command, "%u=%u\n", channel, power);
	write(fd, command, sizeof(command));
}
