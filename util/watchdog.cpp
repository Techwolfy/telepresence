//Watchdog.cpp

//Includes
#include <stdio.h>
#include <chrono>
#include "util/watchdog.h"

//Constructor
Watchdog::Watchdog() : Watchdog(500) {

}

Watchdog::Watchdog(unsigned long timeout, bool silent /* = true */) : interval(timeout),
																	  lastTime(std::chrono::steady_clock::now()),
																	  muted(silent) {
	if(!muted) {
		printf("Watchdog started.\n");
	}
}

//Destructor
Watchdog::~Watchdog() {
	if(!muted) {
		printf("Watchdog stopped.\n");
	}
}

//Functions
//Get watchdog timeout
unsigned long Watchdog::getTimeout() {
	return interval;
}

//Set watchdog timeout
void Watchdog::setTimeout(unsigned long timeout) {
	interval = timeout;
}

//Reset the watchdog timer
void Watchdog::feed() {
	lastTime = std::chrono::steady_clock::now();
}

//Reset the watchdog timer
void Watchdog::reset() {
	feed();
}

//Tell the caller if the watchdog has died
bool Watchdog::isAlive() {
	if((unsigned long)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime)).count() <= interval) {
		return true;
	} else {
		if(!muted) {
			printf("Watchdog died!\n");
		}
		return false;
	}
}

//Tell the caller if the timer duration has elapsed
bool Watchdog::expired() {
	return !isAlive();
}
