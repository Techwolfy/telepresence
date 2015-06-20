#ifndef MOTOR_H
#define MOTOR_H

//Declaration
class Motor {
public:
	//Constructor
	Motor() = default;

	//Destructor
	~Motor() = default;

	//Functions
	virtual void control(double values[]) = 0;
	virtual void stop() = 0;
};

#endif //MOTOR_H