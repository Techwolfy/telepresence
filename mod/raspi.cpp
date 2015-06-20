//RasPi.cpp

//Includes
#include <stdio.h>
#include "raspi.h"
#include "wiringPi.h"
#include "softPwm.h"

//Constructor
RasPi::RasPi() {
	//Set up GPIO
	if(wiringPiSetup() == -1) {	//Must be run as root!
		printf("Error opening GPIO!\n");
		//TODO: Throw exception? Probably segfaults on fail right now.
	}

	//Initialize sofware PWM outputs
	for(int i; i < 10; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
		softPwmCreate(i, 0, 200);	//20ms period
	}
}

//Destructor
RasPi::~RasPi() {
	stop();
}

//Functions
//Set power of all motors
void RasPi::control(double values[]) {
	for(int i = 0; i < (sizeof(values) / sizeof(double)) && i < 10; i++) {
		//1.0ms full reverse, 1.5ms neural, 2.0ms full forward
		softPwmWrite(i, scalePower(values[i]));	//TODO: Test, output range may be wrong
	}
}

//Stop all motors
void RasPi::stop() {
	for(int i = 0; i < 10; i++) {
		softPwmWrite(i, 15);
	}
}

//Scale power from (-1.0, 1.0) to RasPi range of (10, 20)
double RasPi::scalePower(double power) {
	//-1 to 1, -5 to 5, 10 to 20
	return (power * 5) + 15;
}