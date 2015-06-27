#ifndef INPUT_H
#define INPUT_H

class Input {
public:
	//Constructor
	Input() = default;

	//Destructor
	virtual ~Input() {};	//FIXME: Cannot be explicitly defaulted in g++ <= 4.7; see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53613

	//Functions
	virtual int getNumButtons() = 0;
	virtual int getNumAxes() = 0;
	virtual double getAxis(int axis) = 0;
	virtual bool getButton(int button) = 0;
};

#endif //INPUT_H
