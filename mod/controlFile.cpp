//ControlFile.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "controlFile.h"

//Constructor
ControlFile::ControlFile() : ControlFile("pipe") {

}

ControlFile::ControlFile(const char *filename) : file(NULL),
												 buffer{0},
												 numAxes(0),
												 numButtons(0),
												 axes(NULL),
												 buttons(NULL) {
	//Open file
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Error opening control file!\n");
		throw std::runtime_error("couldn't open control file");
	}

	//Allocate and zero-initialize arrays for input data
	fgets(buffer, sizeof(buffer), file);
	sscanf(buffer, "%d,%d", &numAxes, &numButtons);
	axes = (double *)calloc(numAxes, sizeof(double));
	buttons = (bool *)calloc(numButtons, sizeof(bool));

	printf("Opened control file: %s\n", filename);
	update();
}

//Destructor
ControlFile::~ControlFile() {
	fclose(file);
	free(axes);
	free(buttons);
}

//Functions
//Update cached values if anything has changed
void ControlFile::update() {
	if(fgets(buffer, sizeof(buffer), file) == NULL) {
		return;
	}

	//Parse CSV string
	//numAxes,numButtons,axis[0],axis[1],etc,button[0],button[1],etc\n
	int currentPos = 0;
	int tempPos = 0;
	sscanf(buffer, "%*d,%*d,%n", &currentPos);
	for(int i = 0; i < numAxes; i++) {
		sscanf(&buffer[currentPos], "%lf,%n", &axes[i], &tempPos);
		currentPos += tempPos;
	}
	for(int i = 0; i < numButtons; i++) {
		if(buffer[currentPos] == 'T') {
			buttons[i] = true;
		} else {
			buttons[i] = false;
		}
		currentPos += 2;
	}
}

//Retrieve the number of axes on the web client
int ControlFile::getNumAxes() {
	return numAxes;
}

//Retrieve the number of buttons on the web client
int ControlFile::getNumButtons() {
	return numButtons;
}

//Retrieve the value of a specific axis from the web client
double ControlFile::getAxis(int axis) {
	if(axis < numAxes) {
		return axes[axis];
	} else {
		return 0.0;
	}
}

//Retrieve the state of a specific button from the web client
bool ControlFile::getButton(int button) {
	if(button < numButtons) {
		return buttons[button];
	} else {
		return false;
	}
}
