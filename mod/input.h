#ifndef INPUT_H
#define INPUT_H

class Input {
public:
	//Constructor
	Input() = default;

	//Destructor
	~Input() = default;

	//Functions
	virtual int getNumButtons() = 0;
	virtual int getNumAxes() = 0;
	virtual double getAxis(int axis) = 0;
	virtual bool getButton(int button) = 0;
};

#endif //INPUT_H