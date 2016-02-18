#ifndef JOYSTICK_H
#define JOYSTICK_H

//Includes
#include <linux/joystick.h>
#include "control/controller.h"

//Declaration
class Joystick : public Controller {
public:
	//Constructor
	Joystick();
	Joystick(int joyNum);

	//Destructor
	~Joystick();

	//Functions
	void update();
	int getNumAxes();
	int getNumButtons();
	double getAxis(int axis);
	bool getButton(int button);

	//Variables
	static const int JOY_X_AXIS;
	static const int JOY_Y_AXIS;
	static const int JOY_Z_AXIS;

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
