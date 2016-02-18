#ifndef DUMMYJOYSTICK_H
#define DUMMYJOYSTICK_H

//Includes
#include "control/controller.h"
#include "util/ratelimit.h"

//Declaration
class DummyJoystick : public Controller {
public:
	//Constructor
	DummyJoystick();

	//Destructor
	~DummyJoystick();

	//Functions
	void update();
	int getNumButtons();
	int getNumAxes();
	double getAxis(int axis);
	bool getButton(int button);

	//Constants
	static const int JOY_X_AXIS;
	static const int JOY_Y_AXIS;
	static const int JOY_Z_AXIS;

private:
	//Constants
	static const int DUMMY_JOY_AXES;
	static const int DUMMY_JOY_BUTTONS;
	static const double DUMMY_JOY_AXIS_VALUE;
	static const bool DUMMY_JOY_BUTTON_VALUE;

	//Variables
	Ratelimit messages;
	int updateRateID;
};

#endif //DUMMYJOYSTICK_H
