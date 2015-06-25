#ifndef TELEPACKET_H
#define TELEPACKET_H

//Declaration
struct TelePacket {
	//Constants
	const static int NUM_AXES = 3;
	const static int NUM_BUTTONS = 10;

	//Variables
	char head;
	int frameNum;
	bool isClient;
	bool isRobot;
	bool ping;
	double axes[NUM_AXES];
	bool buttons[NUM_BUTTONS];

	//Constructor
	TelePacket(): frameNum(0),
				  isClient(false),
				  isRobot(false),
				  ping(false),
				  head('T') {
		//Zero-initialize axis and button arrays
		for(int i = 0; i < NUM_AXES; i++) {
			axes[i] = 0.0;
		}
		for(int i = 0; i < NUM_BUTTONS; i++) {
			buttons[i] = false;
		}
	}

};

#endif //TELEPACKET_H
