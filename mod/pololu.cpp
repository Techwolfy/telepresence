//Pololu.cpp

//Includes
#include <stdio.h>
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
		//TODO: Throw exception? Probably segfaults on fail right now.
	}
}

//Destructor
Pololu::~Pololu() {
	stop();
	delete pololu;
}

//Functions
//Set power of all motors
void Pololu::control(double values[]) {
	for(int i = 0; i < (sizeof(values) / sizeof(double)) && i < 6; i++) {
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
double scalePower(double power) {
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

/*
#include <signal.h>

	//Set up SIGINT handler
	if(signal(SIGINT, signalHandler) != SIG_ERR) {
		cout << "SIGINT handler intialized!" << endl;
	} else {
		cout << "SIGINT handler intialization failed!" << endl;
		return -1;
	}

	void signalHandler(int signal) {
	if(signal == SIGINT) {
		cout << "SIGINT received!" << endl;
		running = false;
	}
}
*/