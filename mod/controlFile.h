#ifndef CONTROLFILE_H
#define CONTROLFILE_H

//Includes
#include "input.h"

//Declaration
class ControlFile : public Input {
public:
	//Constructor
	ControlFile();

	ControlFile(const char *filename);

	//Destructor
	~ControlFile();

	//Functions
	int getNumButtons();
	int getNumAxes();
	double getAxis(int axis);
	bool getButton(int button);

private:
	//Variables
	FILE *file;
	char buffer[255];
	int numAxes;
	int numButtons;
	double *axes;
	bool *buttons;

	//Functions
	void update();
};

#endif //CONTROLFILE_H
