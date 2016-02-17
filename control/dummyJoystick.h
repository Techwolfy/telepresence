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

	//Variables
	const static int JOY_X_AXIS;
	const static int JOY_Y_AXIS;
	const static int JOY_Z_AXIS;

private:
	//Variables
	const static int DUMMY_JOY_AXES;
	const static int DUMMY_JOY_BUTTONS;
	const static double DUMMY_JOY_AXIS_VALUE;
	const static bool DUMMY_JOY_BUTTON_VALUE;
	Ratelimit messages;
	int updateRateID;
};

#endif //DUMMYJOYSTICK_H
