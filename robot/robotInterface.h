#ifndef ROBOTINTERFACE_H
#define ROBOTINTERFACE_H

//Declaration
class RobotInterface {
public:
	//Constructor
	RobotInterface() = default;

	//Destructor
	virtual ~RobotInterface() {};	//FIXME: Cannot be explicitly defaulted in g++ <= 4.7; see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53613

	//Functions
	virtual void control(int numAxes, double axes[], int numButtons, bool buttons[]) = 0;
	virtual void stop() = 0;
};

extern "C" RobotInterface* createRobot(const char *options);
extern "C" void destroyRobot(RobotInterface *interface);

#endif //ROBOTINTERFACE_H
