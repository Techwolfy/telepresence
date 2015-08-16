#ifndef MOTOR_H
#define MOTOR_H

//Declaration
class Motor {
public:
	//Constructor
	Motor() = default;

	//Destructor
	virtual ~Motor() {};	//FIXME: Cannot be explicitly defaulted in g++ <= 4.7; see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53613

	//Functions
	virtual void control(int numValues, double values[]) = 0;
	virtual void stop() = 0;
};

#endif //MOTOR_H
