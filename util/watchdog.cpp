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
	interval = timeout;
}

void Watchdog::feed() {
	time(&lastTime);
}

bool Watchdog::isAlive() {
	return difftime(time(NULL), lastTime) * 1000 <= interval;
}
