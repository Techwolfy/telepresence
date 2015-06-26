#ifndef JOYSTICK_H
#define JOYSTICK_H

//Includes
#include <linux/joystick.h>
#include "input.h"

//Declaration
class Joystick : public Input {
public:
	//Constructor
	Joystick();

	Joystick(const char *file);

	//Destructor
	~Joystick();

	//Functions
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

	//Functions
	void update();
};

#endif //JOYSTICK_H
