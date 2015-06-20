#ifndef DUMMYJOYSTICK_H
#define DUMMYJOYSTICK_H

//Includes
#include "joystick.h"

class DummyJoystick : public Joystick {
public:
	//Constructor
	DummyJoystick();

	//Destructor
	~DummyJoystick();

private:
	//Functions
	void update();
};

#endif //DUMMYJOYSTICK_H