#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H

//Declaration
class RobotInterface {
public:
	//Constructor
	RobotInterface() = default;

	//Destructor
	virtual ~RobotInterface() = default;

	//Functions
	virtual void control(int numAxes, double axes[], int numButtons, bool buttons[]) = 0;
	virtual void stop() = 0;
};

extern "C" RobotInterface* createRobot(const char *options);
extern "C" void destroyRobot(RobotInterface *interface);

#endif //ROBOTINTERFACE_H
