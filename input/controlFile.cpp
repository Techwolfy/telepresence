//ControlFile.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include "input/controlFile.h"

//Constructor
ControlFile::ControlFile() : ControlFile("pipe") {

}

ControlFile::ControlFile(const char *filename) : file(NULL),
												 buffer{0} {
	//Open file
	file = fopen(filename, "r");
	if(file == NULL) {
		printf("Error opening control file!\n");
		throw std::runtime_error("couldn't open control file");
	}

	printf("Opened control file: %s\n", filename);
	update();
}

//Destructor
ControlFile::~ControlFile() {
	fclose(file);
}

//Functions
//If running from a FIFO, updates cached values if anything has changed
//If running from a file, updates cached values with next line (one line per frame)
void ControlFile::update() {
	if(fgets(buffer, sizeof(buffer), file) == NULL) {
		return;
	}

	//Parse JSON
	reader.parse(buffer, json, false);
}

//Retrieve the number of axes on the web client
int ControlFile::getNumAxes() {
	return json["axes"].size();
}

//Retrieve the number of buttons on the web client
int ControlFile::getNumButtons() {
	return json["buttons"].size();
}

//Retrieve the value of a specific axis from the web client
double ControlFile::getAxis(int axis) {
	return json["axes"].get(axis, 0.0).asDouble();
}

//Retrieve the state of a specific button from the web client
bool ControlFile::getButton(int button) {
	return json["buttons"].get(button, false).asBool();
}
