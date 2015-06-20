//Telepresence server

#include <stdio.h>
#include <unistd.h>
#include "telepacket.h"
#include "lib/super_sock.h"

using namespace std;

TelePacket getData(SuperSock &s);
void sendData(SuperSock &s, TelePacket &data);

int main(int argc, char *argv[]) {
	char *port = (char *)"8353";
	char *address = (char *)"0.0.0.0";

	SuperSock s;
	if(argc >= 2) {
		port = (char *)argv[1];
	}

	//Init socket
	if(s.init(port, address)) {
		printf("Socket initialized.\n");
	} else {
		printf("Socket initialization failed!\n");
		return -1;
	}

	//Main program loop
	while(true) {
		//Get data from client
		TelePacket data = getData(s);
		if(data.head[0] != 'T') {
			usleep(5000);
			continue;
		}
		if(data.isClient) {	//Data recieved from client
			printf("Client frame: %d\n", data.frameNum);
			for(int i = 0; i < (sizeof(data.controls) / sizeof(double)); i++) {
				printf("Control %d: %f\n", i, data.controls[i]);
			}
		} else {
			//The robot doesn't currently send any data, so ignore this.
		}

		//Send data to robot
		sendData(s, data);

		usleep(5000); //0.005 seconds
	}

	return 0;
}

TelePacket getData(SuperSock &s) {
	TelePacket data;
	data.head[0] = '\0';
	s.read_timeout((unsigned char*)&data, sizeof(data), 10000);
	return data;
}

void sendData(SuperSock &s, TelePacket &data) {
	s.write_timeout((unsigned char*)&data, sizeof(data), 10000);
}
