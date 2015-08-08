//Pololu.cpp

//Includes
#include <stdio.h>
#include <stdexcept>
#include "pololu.h"
#include "RPMSerialInterface.h"

//Constructor
Pololu::Pololu() : Pololu("/dev/ttyACM0", 9600) {

}

Pololu::Pololu(const char *file, int baud) {
	//Set up Pololu interface
	pololu = RPM::SerialInterface::createSerialInterface(file, baud);
	if(pololu->isOpen()) {
		printf("Pololu initialized!\n");
	} else {
		printf("Pololu initialization failed!\n");
		throw std::runtime_error("pololu initialization failed");
	}

	//Make sure all motors are stopped
	stop();
}

//Destructor
Pololu::~Pololu() {
	stop();
	delete pololu;
}

//Functions
//Set power of all motors
void Pololu::control(int numValues, double values[]) {
	for(int i = 0; i < numValues && i < 6; i++) {
		pololu->setTargetCP(i, scalePower(values[i]));
	}
}

//Stop all motors
void Pololu::stop() {
	for(int i = 0; i < 6; i++) {
		pololu->setTargetCP(i, 0.0);
	}
}

//Scale power from (-1.0, 1.0) to Pololu range of (4000, 8000)
double Pololu::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (4000, 8000) quarter-of-microseconds; 0 is 6000 (neutral)
	power *= 2000.0;
	power += 6000.0;

	return power;
}
