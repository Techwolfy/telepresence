#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H

//Declaration
class RobotInterface {
public:
	//Destructor
	virtual ~RobotInterface() = default;

	//Functions
	virtual void run(int numAxes, double axes[], int numButtons, bool buttons[]) = 0;
	virtual void stop() = 0;

protected:
	//Constructor ("protected" idenfitier prevents RobotInterface class from being instantiated)
	RobotInterface() = default;
};

extern "C" RobotInterface* createRobot(const char *options);
extern "C" void destroyRobot(RobotInterface *interface);

#endif //ROBOTINTERFACE_H
