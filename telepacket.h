#ifndef TELEPACKET_H
#define TELEPACKET_H

struct TelePacket {
	const static int NUM_AXES = 3;
	const static int NUM_BUTTONS = 10;

	char head;
	int frameNum;
	bool isClient;
	double axes[NUM_AXES];
	bool buttons[NUM_BUTTONS];

	TelePacket(): frameNum(0),
				  isClient(true),
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
