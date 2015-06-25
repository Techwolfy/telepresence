//Telepresence client/server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "base.h"
#include "server.h"
#include "client.h"
#include "robot.h"

void help();

int main(int argc, char *argv[]) {
	bool isClient = false;
	bool isRobot = false;
	char address[16];
	char port[6];
	Base *telepresence = NULL;

	//Process command-line options
	int c = 0;
	while((c = getopt(argc, argv, "hvdscr")) != -1) {
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

	//Create main program object
	if(!isClient && !isRobot) {
		telepresence = new Server(address, port);
	} else if(isClient) {
		telepresence = new Client(address, port);
	} else {
		telepresence = new Robot(address, port);
	}

	//Infinite loop
	while(true) {
		telepresence->run();
		usleep(5000); //0.005 seconds
	}

	delete telepresence;

	return 0;
}

//Output a help message to the console
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
