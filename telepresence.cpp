//Telepresence client
//TODO: Web module

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "telepacket.h"
#include "lib/super_sock.h"

#ifdef JOYSTICK
	#include "mod/joystick.h"
#else
	#include "mod/dummyJoystick.h"
#endif

#include "mod/motor.h"
#ifdef POLOLU
	#include "mod/pololu.h"
#elif RASPI
	#include "mod/raspi.h"
#else
	#include "mod/dummyMotor.h"
#endif

void server(SuperSock &s);
void client(SuperSock &s);
void robot(SuperSock &s);
void help();
TelePacket getData(SuperSock &s);
void sendData(SuperSock &s, TelePacket &data);
void printData(TelePacket &data);

int main(int argc, char *argv[]) {
	bool isClient = false;
	bool isRobot = false;
	char address[16];
	char port[6];
	SuperSock s;

	//Process command-line options
	int c = 0;
	while((c = getopt (argc, argv, "hvscr")) != -1) {
		switch(c) {
			case 's':	//Server mode
				isClient = false;
				isRobot = false;
			case 'c':	//Client mode
				isClient = true;
				isRobot = false;
				break;
			case 'r':	//Robot mode
				isClient = false;
				isRobot = true;
				break;
			case '?':
				printf("Unknown option `%c'.\n", optopt);
				return 1;
			case 'h':	//Help
			case 'v':	//Version
			default:
				help();
				return 1;
		}
	}

	//Process host:port parameter (first non-option argument), ignore the rest
	if(optind < argc) {
		strcpy(address, strtok(argv[optind], ":"));
		strcpy(port, strtok(NULL, ":"));
		if(address == NULL || port == NULL) {
			printf("Invalid address / port! Exiting.\n");
			return 1;
		}
	} else {
		if(!isClient && !isRobot) {
			printf("Missing address / port! Using defaults (0.0.0.0:8353).\n");
			sprintf(address, "0.0.0.0");
			sprintf(port, "8353");
		} else {
			printf("Missing address / port! Using defaults (127.0.0.1:8353).\n");
			sprintf(address, "127.0.0.1");
			sprintf(port, "8353");
		}
	}

	//Init socket
	if(s.init(port, address)) {
		printf("Socket initialized.\n");
	} else {
		printf("Socket initialization failed!\n");
		return -1;
	}

	//Start main program loop
	if(!isClient && !isRobot) {
		server(s);
	} else if(isClient) {
		client(s);
	} else {
		robot(s);
	}

	return 0;
}

void server(SuperSock &s) {
	TelePacket data;

	//Main server loop
	while(true) {
		//Get data from client
		data = getData(s);
		if(data.head != 'T') {
			usleep(5000);
			continue;
		}

		if(data.isClient) {	//Data recieved from client
			printData(data);
		} else {
			//The robot doesn't currently send any data, so ignore this.
		}

		//Send data to robot
		sendData(s, data);

		usleep(5000); //0.005 seconds
	}
}

void client(SuperSock &s) {
	TelePacket out;
	out.frameNum = 0;
	out.isClient = true;

#ifdef JOYSTICK
	Joystick joy;
#else
	DummyJoystick joy;
#endif

	//Main client loop
	while(true) {
		//Send joystick data to robot
		out.frameNum++;
		for(int i = 0; i < joy.getNumAxes() && i < TelePacket::NUM_AXES; i++) {
			out.axes[i] = joy.getAxis(i);
		}
		for(int i = 0; i < joy.getNumButtons() && i < TelePacket::NUM_BUTTONS; i++) {
			out.buttons[i] = joy.getButton(i);
		}
		sendData(s, out);

		usleep(5000); //0.005 seconds
	}
}

void robot(SuperSock &s) {
	TelePacket data;

#ifdef POLOLU
	Pololu motor;
#elif RASPI
	RasPi motor;
#else
	DummyMotor motor;
#endif

	//Main robot loop
	while(true) {
		//Get control data from server
		data = getData(s);
		if(data.head != 'T') {
			usleep(5000);
			continue;
		}

		if(data.isClient) {	//Data recieved from client
			printData(data);
			motor.control(data.axes);
		}

		usleep(5000); //0.005 seconds
	}	
}

void help() {
	printf("Telepresence client v1.0.0 by Daniel Ring\n");
	printf("Usage: telepresence [options] address:port\n");
	printf("-h\tPrints this help message.\n");
	printf("-v\tPrints this version message.\n");
	printf("-s\tRun in server mode (relaying data).\n");
	printf("-c\tRun in client mode (controlling a robot).\n");
	printf("-r\tRun in robot mode (controlled by a client).\n");
}

TelePacket getData(SuperSock &s) {
	TelePacket data;
	data.head = '\0';
	s.read_timeout((unsigned char*)&data, sizeof(data), 10000);
	return data;
}

void sendData(SuperSock &s, TelePacket &data) {
	s.write_timeout((unsigned char*)&data, sizeof(data), 10000);
}

void printData(TelePacket &data) {
	printf("Client frame: %d\n", data.frameNum);
	for(int i = 0; i < TelePacket::NUM_AXES; i++) {
		printf("Axis %d: %f\n", i, data.axes[i]);
	}
	for(int i = 0; i < TelePacket::NUM_BUTTONS; i++) {
		printf("Button %d: %c\n", i, data.buttons[i] ? 'T' : 'F');
	}
}