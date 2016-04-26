//DummyRobot.cpp

//Includes
#include "robot/dummyRobot.h"
#include "util/log.h"
#include "util/ratelimit.h"

//Constructor
DummyRobot::DummyRobot() : messages(500, 1) {
	runRateID = messages.getID();
	stopRateID = messages.getID();

	Log::logf(Log::INFO, "Dummy robot created.\n");
}

//Destructor
DummyRobot::~DummyRobot() {
	Log::logf(Log::INFO, "Dummy robot destroyed.\n");
}

//Shared library constructor
extern "C" RobotInterface* createRobot(const char *options) {
	return new DummyRobot();
}

//Shared library destructor
extern "C" void destroyRobot(RobotInterface *interface) {
	delete interface;
}

//Functions
//Dummy robot; output values to console
void DummyRobot::run(int numAxes, double axes[], int numButtons, bool buttons[]) {
	if(!messages.limitReached(runRateID)) {
		Log::logf(Log::DEBUG, "Dummy robot run() called.\n");
		for(int i = 0; i < numAxes; i++) {
			Log::logf(Log::DEBUG, "Axis %d: %f\n", i, axes[i]);
		}
		for(int i = 0; i < numButtons; i++) {
			Log::logf(Log::DEBUG, "Button %d: %c\n", i, buttons[i] ? 'T' : 'F');
		}
		messages.increment(runRateID);
	}
}

//Dummy robot; output stop message to console
void DummyRobot::stop() {
	if(!messages.limitReached(stopRateID)) {
		Log::logf(Log::DEBUG, "Dummy robot stop() called.\n");
		messages.increment(stopRateID);
	}
}
