//Telepresence client
//TODO: Web module

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "telepacket.h"
#include "udpsocket.h"

#include "mod/input.h"
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

void server(UDPSocket *s, TelePacket *data);
void client(UDPSocket *s, TelePacket *data);
void robot(UDPSocket *s, TelePacket *data);
void help();
void printData(TelePacket *data);

int main(int argc, char *argv[]) {
	bool isClient = false;
	bool isRobot = false;
	char address[16];
	char port[6];
	UDPSocket s;
	TelePacket data;

	//Process command-line options
	int c = 0;
	while((c = getopt (argc, argv, "hvdscr")) != -1) {
		switch(c) {
			case 'd':	//Daemon mode
			case 's':	//Server mode
				isClient = false;
				isRobot = false;
				break;
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
	if(s.openSocket(address, port) < 0) {
		printf("Socket initialization failed!\n");
		return -1;
	} else {
		printf("Socket initialized.\n");
	}

	//Start main program loop
	if(!isClient && !isRobot) {
		server(&s, &data);
	} else if(isClient) {
		client(&s, &data);
	} else {
		robot(&s, &data);
	}

	return 0;
}

void server(UDPSocket *s, TelePacket *data) {
	struct sockaddr_in *robotAddress;
	struct sockaddr_in *clientAddress;
	struct sockaddr_in *unknownAddress;

	//Main server loop
	while(true) {
		//Get data from stream
		data->head = '\0';
		unknownAddress = s->readData((void *)data, sizeof(*data));
		if(unknownAddress == NULL || data->head != 'T') {
			usleep(5000);
			continue;
		}

		if(data->isClient) {	//Data recieved from client
			clientAddress = unknownAddress;
			printData(data);
		} else {
			robotAddress = unknownAddress;
			//The robot doesn't currently send any useful data.
		}

		//Send data to robot
		s->writeData(robotAddress, (void *)data, sizeof(*data));

		usleep(5000); //0.005 seconds
	}
}

void client(UDPSocket *s, TelePacket *data) {
	data->frameNum = 0;
	data->isClient = true;

#ifdef JOYSTICK
	Joystick joy;
#else
	DummyJoystick joy;
#endif

	//Main client loop
	while(true) {
		//Prepare joystick data for robot
		data->frameNum++;
		for(int i = 0; i < joy.getNumAxes() && i < TelePacket::NUM_AXES; i++) {
			data->axes[i] = joy.getAxis(i);
		}
		for(int i = 0; i < joy.getNumButtons() && i < TelePacket::NUM_BUTTONS; i++) {
			data->buttons[i] = joy.getButton(i);
		}

		//Send data to robot
		s->writeData((void *)data, sizeof(*data));

		usleep(5000); //0.005 seconds
	}
}

void robot(UDPSocket *s, TelePacket *data) {
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
		data->head = '\0';
		s->readData((void *)data, sizeof(*data));
		if(data->head != 'T') {
			usleep(5000);
			continue;
		}

		if(data->isClient) {	//Data recieved from client
			printData(data);
			motor.control(TelePacket::NUM_AXES, data->axes);
		}

		//TODO: Send some data to the server so it can respond with commands

		usleep(5000); //0.005 seconds
	}	
}

void help() {
	printf("Telepresence client v1.0.0 by Daniel Ring\n");
	printf("Usage: telepresence [options] address:port\n");
	printf("-h\tPrints this help message.\n");
	printf("-v\tPrints this version message.\n");
	printf("-d\tRun in server mode (relaying data).\n");
	printf("-s\tRun in server mode (same as -d).\n");
	printf("-c\tRun in client mode (controlling a robot).\n");
	printf("-r\tRun in robot mode (controlled by a client).\n");
}

void printData(TelePacket *data) {
	printf("Client frame: %d\n", data->frameNum);
	for(int i = 0; i < TelePacket::NUM_AXES; i++) {
		printf("Axis %d: %f\n", i, data->axes[i]);
	}
	for(int i = 0; i < TelePacket::NUM_BUTTONS; i++) {
		printf("Button %d: %c\n", i, data->buttons[i] ? 'T' : 'F');
	}
}