//Telepresence client/server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <exception>
#include <stdexcept>
#include "server.h"
#include "client.h"
#include "robot.h"

void help();

int main(int argc, char *argv[]) {
	bool isClient = false;
	bool isRobot = false;
	char host[255] = "0.0.0.0";
	char port[6] = "8353";
	Server *telepresence = NULL;
	bool dummy = false;
	int clientJoystick = -1;
	char file[255] = {0};

	//Process command-line options
	int c = 0;
	while((c = getopt(argc, argv, "hvscra:p:dj:f:")) != -1) {
		switch(c) {
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
			case 'a':	//Remote host
				strcpy(host, optarg);
				break;
			case 'p':	//Remote port
				strcpy(port, optarg);
				break;
			case 'd':	//Dummy I/O
				dummy = true;
				break;
			case 'j':	//Joystick number [Client only]
				clientJoystick = atoi(optarg);
				break;
			case 'f':	//File path [Client only]
				strcpy(file, optarg);
				break;
			case '?':	//Unknown or malformed option
				if(optopt == 'p' || optopt == 'j' || optopt == 'f') {
					printf("Option `-%c' requires an argument.\n", optopt);
				} else {
					printf("Unknown option `-%c'.\n\n", optopt);
				}
				help();
				return EXIT_SUCCESS;
			case 'h':	//Help
			case 'v':	//Version
			default:	//Should never be reached
				help();
				return EXIT_FAILURE;
		}
	}

	//Ensure host parameter (first non-option argument) was provided if necessary
	if(isRobot && strcmp(host, "0.0.0.0") == 0) {
		fprintf(stderr, "Missing hostname! Exiting.\n");
		return EXIT_FAILURE;
	}

	//Create main program object
	try {
		if(!isClient && !isRobot) {
			telepresence = new Server(host, port);	//Server doesn't need to explicitly connect to anything
		} else if(isClient) {
			if(dummy) {
				telepresence = new Client(host, port, dummy);
			} else if(clientJoystick >= 0) {
				telepresence = new Client(host, port, clientJoystick);
			} else if(file[0] != 0) {
				telepresence = new Client(host, port, file);
			} else {
				printf("Client input type not specified!\n");
				throw std::runtime_error("missing client input type");
			}
		} else {
			telepresence = new Robot(host, port);
		}
	} catch(std::exception& e) {
		fprintf(stderr, "Fatal error creating main program object (%s), exiting!\n", e.what());
		return EXIT_FAILURE;
	}

	//Infinite loop
	while(true) {
		telepresence->run();
		usleep(5000); //0.005 seconds
	}

	delete telepresence;

	return EXIT_SUCCESS;
}

//Output a help message to the console
void help() {
	printf("Telepresence client v1.0.0 by Daniel Ring\n");
	printf("Usage:\ttelepresence [mode] [options]\n");
	printf("-h\tPrints this help message.\n");
	printf("-v\tPrints this version message.\n");
	printf("-s\tRun in server mode (relaying data).\n");
	printf("-c\tRun in client mode (controlling a robot).\n");
	printf("-r\tRun in robot mode (controlled by a client).\n");
	printf("-a [address]\tRemote server address (required for robot mode; default: autodetect).\n");
	printf("-p [port]\tUse alternate port (default: 8353).\n");
	printf("-d\tUse dummy inputs (client only).\n");
	printf("-j [joystick]\tSpecify an alternade joystick (client only; default: /dev/input/js0).\n");
	printf("-f [file]\tSpecify a file or named pipe to read data from (client only).\n");
}
