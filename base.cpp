//Base.cpp

//Includes
#include <stdio.h>
#include <stdexcept>
#include "base.h"
#include "telepacket.h"
#include "udpsocket.h"

//Constructor
Base::Base(const char *address, const char *port) {
	//Init socket
	if(s.openSocket(address, port) < 0) {
		printf("Socket initialization failed!\n");
		throw std::runtime_error("socket initialization failed");
	} else {
		printf("Socket initialized.\n");
	}
}

//Destructor
Base::~Base() {
	s.closeSocket();
}

//Print the contents of a data packet to the console
void Base::printData(TelePacket &data) {
	printf("Client frame: %d\n", data.frameNum);
	for(int i = 0; i < TelePacket::NUM_AXES; i++) {
		printf("Axis %d: %f\n", i, data.axes[i]);
	}
	for(int i = 0; i < TelePacket::NUM_BUTTONS; i++) {
		printf("Button %d: %c\n", i, data.buttons[i] ? 'T' : 'F');
	}
}
