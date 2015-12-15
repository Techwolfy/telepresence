#ifndef CONTROLFILE_H
#define CONTROLFILE_H

//Includes
#include <jsoncpp/json/json.h>
#include "input/input.h"

//Declaration
class ControlFile : public Input {
public:
	//Constructor
	ControlFile();
	ControlFile(const char *filename);

	//Destructor
	~ControlFile();

	//Functions
	void update();
	int getNumAxes();
	int getNumButtons();
	double getAxis(int axis);
	bool getButton(int button);

private:
	//Variables
	FILE *file;
	char buffer[255];
	Json::Reader reader;
	Json::Value json;
};

#endif //CONTROLFILE_H
