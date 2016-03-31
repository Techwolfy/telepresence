#ifndef JOYSTICK_H
#define JOYSTICK_H

//Includes
#ifndef _WIN32	//Linux/POSIX support
	#include <linux/joystick.h>
#else			//Windows support
	#define WIN32_LEAN_AND_MEAN	//Don't include winsock etc.
	#include <windows.h>
#endif
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

private:
	//Variables
	int joyFD;
#ifndef _WIN32
	struct js_event joyEvent;
#else
	JOYINFOEX joyEvent;
#endif
	int numAxes;
	int numButtons;
	int *axes;
	bool *buttons;
	char name[128];
};

#endif //JOYSTICK_H
