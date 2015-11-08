//Pololu.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include "output/pololu.h"

//Constructor
Pololu::Pololu() : Pololu("/dev/ttyACM0") {

}

Pololu::Pololu(const char *file) {
	//Set up Pololu interface
	pololuFD = open(file, O_RDWR | O_NOCTTY);
	if(pololuFD < 0) {
		printf("Pololu initialization failed!\n");
		throw std::runtime_error("pololu initialization failed");
	}

	printf("Pololu initialized!\n");

	//Make sure all motors are stopped
	stop();
}

//Destructor
Pololu::~Pololu() {
	stop();
	close(pololuFD);
}

//Functions
//Set power of all motors
void Pololu::control(int numValues, double values[]) {
	for(int i = 0; i < numValues && i < 6; i++) {
		setPower(i, scalePower(values[i]));
	}
}

//Stop all motors
void Pololu::stop() {
	for(int i = 0; i < 6; i++) {
		setPower(i, scalePower(0));
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

void Pololu::setPower(unsigned char channel, unsigned short power) {
	if(channel < 0 || channel > 6) {
		printf("Invalid pololu channel!\n");
		return;
	}
	unsigned char command[4] = {0x84, channel, power & 0x7F, (power >> 7) & 0x7F};
	if(write(pololuFD, &command, sizeof(command)) != 4) {
		printf("Error writing to pololu!\n");
	}
}
