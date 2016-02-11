#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
	//Constructor
	Controller() = default;

	//Destructor
	virtual ~Controller() {};	//FIXME: Cannot be explicitly defaulted in g++ <= 4.7; see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53613

	//Functions
	virtual void update() = 0;
	virtual int getNumButtons() = 0;
	virtual int getNumAxes() = 0;
	virtual double getAxis(int axis) = 0;
	virtual bool getButton(int button) = 0;
};

#endif //CONTROLLER_H
