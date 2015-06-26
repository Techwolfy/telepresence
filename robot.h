#ifndef ROBOT_H
#define ROBOT_H

//Includes
#include "base.h"
#include "mod/motor.h"

//Declaration
class Robot : public Base {
public:
	//Constructor
	Robot();

	Robot(const char *address, const char *port);

	//Destructor
	~Robot();

	//Functions
	void run();
	void sendPing();

private:
	//Variables
	Motor *motor;
};

#endif //ROBOT_H
