//Ratelimit.cpp

//Includes
#include <map>
#include "util/ratelimit.h"

//Constructor
Ratelimit::Ratelimit() : Ratelimit(500, 1) {

}

Ratelimit::Ratelimit(unsigned long timeout, int operationsPerInterval) : Watchdog(timeout, true),
																		 limit(operationsPerInterval),
																		 nextID(0) {

}

//Destructor
Ratelimit::~Ratelimit() {

}

//Functions
//Return the maximum number of operations per interval
int Ratelimit::getLimit() {
	return limit;
}

//Return a new rate-limited ID
int Ratelimit::getID() {
	idMap[nextID] = 0;
	return nextID++;
}

//Increment the operation counter of the provided ID
void Ratelimit::increment(int id) {
	//Don't create the ID if it doesn't exist
	if(idMap.find(id) != idMap.end()) {
		idMap[id]++;
	}
}

//Check if the provided ID has hit its rate limit
bool Ratelimit::limitReached(int id) {
	//Reset all IDs if time has elapsed
	if(expired()) {
		for(std::map<int, int>::iterator it = idMap.begin(); it != idMap.end(); it++){
			it->second = 0;
		}
		reset();
	}

	//Don't create the ID if it doesn't exist
	if(idMap.find(id) == idMap.end()) {
		//IDs not being tracked are always muted
		return true;
	}

	return idMap[id] < limit;
}

//Reset the current limit of the provided ID
void Ratelimit::resetLimit(int id) {
	//Don't create the ID if it doesn't exist
	if(idMap.find(id) != idMap.end()) {
		idMap[id] = 0;
	}
}

//Stop rate-limiting the provided ID (limitReached will always be true);
void Ratelimit::dropID(int id) {
	idMap.erase(id);
}
