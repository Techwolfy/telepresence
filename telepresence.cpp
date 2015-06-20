//Telepresence client
//TODO: Web module

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "telepacket.h"
#include "lib/super_sock.h"
#include "mod/joystick.h"
#include "mod/motor.h"

#ifdef DUMMY
	#include "mod/dummyMotor.h"
#elif POLOLU
	#include "mod/pololu.h"
#elif RASPI
	#include "mod/raspi.h"
#else
	#error "Use -DDUMMY for a dummy motor, -DPOLOLU for a 6-port Pololu device, or -DRASPI for a Raspberry Pi."
#endif

void server(SuperSock &s);
void client(SuperSock &s);
void robot(SuperSock &s);
void help();
TelePacket getData(SuperSock &s);
void sendData(SuperSock &s, TelePacket &data);

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
}

void client(SuperSock &s) {
	TelePacket out;
	out.frameNum = 0;
	out.isClient = true;

	Joystick joy;

	out.numAxes = joy.getNumAxes();
	out.controls = (double *)calloc(joy.getNumAxes() + joy.getNumButtons(), sizeof(double));	//TODO: Will this send the array or the pointer?

	//Main client loop
	while(true) {
		//Send joystick data to robot
		out.frameNum++;
		for(int i = 0; i < joy.getNumAxes(); i++) {
			out.controls[i] = joy.getAxis(i);
		}
		for(int i = joy.getNumAxes(); i < (joy.getNumAxes() + joy.getNumButtons()); i++) {
			out.controls[i] = joy.getButton(i - joy.getNumAxes()) ? 1.0 : 0.0;
		}
		sendData(s, out);

		usleep(5000); //0.005 seconds
	}

	free(out.controls);
}

void robot(SuperSock &s) {
	TelePacket data;
#ifdef DUMMY
	DummyMotor motor;
#elif POLOLU
	Pololu motor;
#elif RASPI
	RasPi motor;
#else
	#error "Use -DPOLOLU for a 6-port Pololu device or -DRASPI for a Raspberry Pi. Other options are currently unsupported."
#endif

	//Main robot loop
	while(true) {
		//Get control data from server
		data = getData(s);
		if(data.head[0] != 'T') {
			usleep(5000);
			continue;
		}
		if(data.isClient) {	//Data recieved from client
			printf("Client frame: %d\n", data.frameNum);
			for(int i = 0; i < (sizeof(data.controls) / sizeof(double)); i++) {
				printf("Control %d: %f\n", i, data.controls[i]);
			}
			motor.control(data.controls);
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
	data.head[0] = '\0';
	s.read_timeout((unsigned char*)&data, sizeof(data), 10000);
	return data;
}

void sendData(SuperSock &s, TelePacket &data) {
	s.write_timeout((unsigned char*)&data, sizeof(data), 10000);
}
