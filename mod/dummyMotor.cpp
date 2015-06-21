//DummyMotor.cpp

//Includes
#include <stdio.h>
#include "dummyMotor.h"

//Constructor
DummyMotor::DummyMotor() {
	printf("Dummy motor created.\n");
}

//Destructor
DummyMotor::~DummyMotor() {
	printf("Dummy motor destroyed.\n");
}

//Functions
//Dummy motor; output target values to console
void DummyMotor::control(int numValues, double values[]) {
	printf("Dummy motor control() called.\n");
	for(int i = 0; i < numValues && i < 6; i++) {
		printf("Output %d: %f\n", i, values[i]);
	}
}

//Dummy motor; output stop message to console
void DummyMotor::stop() {
	printf("Dummy motor stop() called.\n");
}
