#ifndef WATCHDOG_H
#define WATCHDOG_H

//Includes
#include <time.h>

//Declaration
class Watchdog {
public:
	//Constructor
	Watchdog();
	Watchdog(unsigned long timeout);

	//Destructor
	~Watchdog();

	//Functions
	void setTimeout(unsigned long timeout);
	void feed();
	bool isAlive();

private:
	//Variables
	double interval;
	time_t lastTime;
};

#endif //WATCHDOG_H
