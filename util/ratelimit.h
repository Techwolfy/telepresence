#ifndef RATELIMIT_H
#define RATELIMIT_H

//Includes
#include <map>
#include "util/watchdog.h"

//Declaration
class Ratelimit : public Watchdog {
public:
	//Constructor
	Ratelimit();
	Ratelimit(unsigned long interval, int operationsPerInterval);

	//Destructor
	~Ratelimit();

	//Functions
	int getLimit();
	int getID();
	void increment(int id);
	bool limitReached(int id);
	void resetLimit(int id);
	void dropID(int id);

private:
	//Variables
	int limit;
	int nextID;
	std::map<int, int> idMap;
};

#endif //RATELIMIT_H
