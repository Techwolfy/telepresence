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

private:
	//Constants
	static const int DUMMY_JOY_AXES = 3;
	static const int DUMMY_JOY_BUTTONS = 10;
	static constexpr double DUMMY_JOY_AXIS_VALUE = 0.5;
	static const bool DUMMY_JOY_BUTTON_VALUE = true;

	//Variables
	Ratelimit messages;
	int updateRateID;
};

#endif //DUMMYJOYSTICK_H
