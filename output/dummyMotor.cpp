//DummyMotor.cpp

//Includes
#include <stdio.h>
#include "output/dummyMotor.h"
#include "util/watchdog.h"

//Constructor
DummyMotor::DummyMotor() : ratelimitControl(500),
						   ratelimitStop(500) {
	printf("Dummy motor created.\n");
}

//Destructor
DummyMotor::~DummyMotor() {
	printf("Dummy motor destroyed.\n");
}

//Functions
//Dummy motor; output target values to console
void DummyMotor::control(int numValues, double values[]) {
	if(ratelimitControl.expired()) {
		printf("Dummy motor control() called.\n");
		for(int i = 0; i < numValues; i++) {
			printf("Output %d: %f\n", i, values[i]);
		}
		ratelimitControl.reset();
	}
}

//Dummy motor; output stop message to console
void DummyMotor::stop() {
	if(ratelimitStop.expired()) {
		printf("Dummy motor stop() called.\n");
		ratelimitStop.reset();
	}
}
