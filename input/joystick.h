#ifndef JOYSTICK_H
#define JOYSTICK_H

//Includes
#include <linux/joystick.h>
#include "input/input.h"

//Declaration
class Joystick : public Input {
public:
	//Constructor
	Joystick();
	Joystick(int joyNum);

	//Destructor
	~Joystick();

	//Functions
	void update();
	int getNumButtons();
	int getNumAxes();
	double getAxis(int axis);
	bool getButton(int button);

	//Variables
	const static int JOY_X_AXIS;
	const static int JOY_Y_AXIS;
	const static int JOY_Z_AXIS;

private:
	//Variables
	int joyFD;
	struct js_event joyEvent;
	int numAxes;
	int numButtons;
	int *axes;
	bool *buttons;
	char name[128];
};

#endif //JOYSTICK_H
