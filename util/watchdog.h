#ifndef WATCHDOG_H
#define WATCHDOG_H

//Includes
#include <chrono>

//Declaration
class Watchdog {
public:
	//Constructor
	Watchdog();
	Watchdog(unsigned long timeout, bool silent = true);

	//Destructor
	~Watchdog();

	//Functions
	unsigned long getTimeout();
	void setTimeout(unsigned long timeout);
	void feed();
	void reset();
	bool isAlive();
	bool expired();

private:
	//Variables
	unsigned long interval;
	std::chrono::steady_clock::time_point lastTime;
	bool muted;
};

#endif //WATCHDOG_H
