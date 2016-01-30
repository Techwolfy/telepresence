#ifndef OUTPUT_H
#define OUTPUT_H

//Declaration
class Output {
public:
	//Constructor
	Output() = default;

	//Destructor
	virtual ~Output() {};	//FIXME: Cannot be explicitly defaulted in g++ <= 4.7; see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53613

	//Functions
	virtual void control(int numAxes, double axes[], int numButtons, bool buttons[]) = 0;
	virtual void stop() = 0;
};

extern "C" Output* createRobot(const char *options);
extern "C" void destroyRobot(Output *output);

#endif //OUTPUT_H
