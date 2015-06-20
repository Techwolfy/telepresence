#ifndef JOYSTICK_H
#define JOYSTICK_H

//Includes
#include <linux/joystick.h>

class Joystick {
public:
	//Constructor
	Joystick();

	Joystick(const char *file);

	//Destructor
	~Joystick();

	//Functions
	int getNumButtons();
	int getNumAxes();
	double getX();
	double getY();
	double getZ();
	double getAxis(int axis);
	bool getButton(int button);

protected:
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