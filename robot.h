#ifndef ROBOT_H
#define ROBOT_H

//Includes
#include "base.h"

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
};

#endif //ROBOT_H
