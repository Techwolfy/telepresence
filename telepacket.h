#ifndef TELEPACKET_H
#define TELEPACKET_H


struct TelePacket {
	char head[13];
	int frameNum;
	bool isClient;
	int numAxes;
	double *controls;

	TelePacket(): frameNum(0),
				  isClient(true),
				  numAxes(0),
				  controls(NULL) {
		head[0] = 'T';
		head[1] = 'e';
		head[2] = 'l';
		head[3] = 'e';
		head[4] = 'p';
		head[5] = 'r';
		head[6] = 'e';
		head[7] = 's';
		head[8] = 'e';
		head[9] = 'n';
		head[10] = 'c';
		head[11] = 'e';
		head[12] = '\0';
	}
		
};

#endif //TELEPACKET_H
