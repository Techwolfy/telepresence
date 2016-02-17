//RasPi.cpp

//Includes
#include <stdio.h>
#include <stdexcept>
#include "device/raspi.h"
#include "lib/wiringPi/static/include/wiringPi.h"
#include "lib/wiringPi/static/include/softPwm.h"

//Constructor
RasPi::RasPi() {
	//Set up GPIO
	if(wiringPiSetup() == -1) {	//Must be run as root!
		printf("GPIO initialization failed!\n");
		throw std::runtime_error("GPIO initialization failed");
	}

	//Initialize sofware PWM outputs
	for(int i; i < 10; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
		softPwmCreate(i, 0, 200);	//20ms period
	}

	//Make sure all motors are stopped
	stopMotors();
}

//Destructor
RasPi::~RasPi() {
	stopMotors();
}

//Functions
//Set power of a specific motor
void RasPi::setMotorPower(int motorNum, double power) {
	if(motorNum < 0 || motorNum > RASPI_NUM_MOTORS) {
		printf("Invalid motor GPIO pin!\n");
		return;
	}

	//1.0ms full reverse, 1.5ms neural, 2.0ms full forward
	softPwmWrite(motorNum, scalePower(power));	//TODO: Test, output range may be wrong
}

//Stop all motors
void RasPi::stopMotors() {
	for(int i = 0; i < RASPI_NUM_MOTORS; i++) {
		softPwmWrite(i, scalePower(0.0));
	}
}

//Scale power from (-1.0, 1.0) to RasPi range of (10, 20)
double RasPi::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//-1 to 1, -5 to 5, 10 to 20
	return (power * 5) + 15;
}
