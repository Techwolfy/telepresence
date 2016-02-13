#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
	//Constructor
	Controller() = default;

	//Destructor
	virtual ~Controller() = default;

	//Functions
	virtual void update() = 0;
	virtual int getNumButtons() = 0;
	virtual int getNumAxes() = 0;
	virtual double getAxis(int axis) = 0;
	virtual bool getButton(int button) = 0;
};

#endif //CONTROLLER_H
