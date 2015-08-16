//Watchdog.cpp

//Includes
#include <stdio.h>
#include <time.h>
#include "util/watchdog.h"
#include "telepacket.h"

//Constructor
Watchdog::Watchdog() : Watchdog(500) {

}

Watchdog::Watchdog(unsigned long timeout) : interval(timeout),
											lastTime{NULL} {
	printf("Watchdog started.\n");
}

//Destructor
Watchdog::~Watchdog() {
	printf("Watchdog stopped.\n");
}

//Functions
void Watchdog::setTimeout(unsigned long timeout) {
	interval = (double)timeout / 1000.0;
}

void Watchdog::feed() {
	time(&lastTime);
}

bool Watchdog::isAlive() {
	if(difftime(time(NULL), lastTime) <= interval) {
		return true;
	} else {
		printf("Watchdog died!\n");
		return false;
	}
}
