//Telepresence client/server

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <exception>
#include <stdexcept>
#include <chrono>
#include "server.h"
#include "client.h"
#include "robot.h"

//Function declarations
void help();
void signalHandler(int signal);

//Global variable for main loop control
bool running;

//Functions
int main(int argc, char *argv[]) {
	running = false;
	long elapsedTime = 0;
	std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
	bool isClient = false;
	bool isRobot = false;
	char host[255] = "0.0.0.0";
	char port[6] = "8353";
	bool listen = false;
	Server *telepresence = NULL;
	bool dummy = false;
	int clientJoystick = -1;
	char file[255] = {0};
	char libFile[255] = {0};

	//Process command-line options
	int c = 0;
	while((c = getopt(argc, argv, "hvscrla:p:dj:f:o:")) != -1) {
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
			case 'l':	//Listening mode
				listen = true;
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
			case 'o':
				strcpy(libFile, optarg);
				break;
			case '?':	//Unknown or malformed option
				if(optopt == 'p' || optopt == 'j' || optopt == 'f' || optopt == 'o') {
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

	//Check if local client port should be ephemeral
	if((isClient || isRobot) && !listen && strcmp(host, "0.0.0.0") == 0) {
			strcpy(host, "127.0.0.1");
	}

	//Set up SIGINT handler
	if(signal(SIGINT, signalHandler) != SIG_ERR) {
		printf("SIGINT handler intialized!\n");
	} else {
		printf("SIGINT handler intialization failed!\n");
		return EXIT_FAILURE;
	}

	//Create main program object
	try {
		if(!isClient && !isRobot) {
			telepresence = new Server(host, port);
		} else if(isClient) {
			if(dummy) {
				telepresence = new Client(host, port, listen, dummy);
			} else if(clientJoystick >= 0) {
				telepresence = new Client(host, port, listen, clientJoystick);
			} else if(file[0] != 0) {
				telepresence = new Client(host, port, listen, file);
			} else {
				printf("Client input type not specified!\n");
				throw std::runtime_error("missing client input type");
			}
		} else {
			if(dummy) {
				telepresence = new Robot(host, port, listen);
			} else if(libFile[0] != 0) {
				telepresence = new Robot(host, port, listen, libFile);
			} else {
				printf("Robot output type not specified!\n");
				throw std::runtime_error("missing robot output type");
			}
		}
	} catch(std::exception &e) {
		fprintf(stderr, "Fatal error creating main program object (%s), exiting!\n", e.what());
		return EXIT_FAILURE;
	}

	//Infinite loop
	running = true;
	while(running) {
		startTime = std::chrono::steady_clock::now();
		telepresence->run();
		elapsedTime = (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime)).count();
		if(20000 - elapsedTime > 0) {
			usleep(20000 - elapsedTime);	//Remainder of 20 milliseconds, if any
		}
	}

	delete telepresence;

	return EXIT_SUCCESS;
}

//Output a help message to the console
void help() {
	printf("Telepresence v1.0.0 by Daniel Ring\n");
	printf("Usage:\ttelepresence [mode] [options]\n");
	printf("-h\t\tPrints this help message.\n");
	printf("-v\t\tPrints this version message.\n");
	printf("-s\t\tRun in server mode (relaying data).\n");
	printf("-c\t\tRun in client mode (controlling a robot).\n");
	printf("-r\t\tRun in robot mode (controlled by a client).\n");
	printf("-l\t\tListening mode (client/robot only; default: off).\n");
	printf("-a [address]\tLocal listening address / remote server address (default: 0.0.0.0 / 127.0.0.1).\n");
	printf("-p [port]\tLocal listening port / remote server port (default: 8353).\n");
	printf("-d\t\tUse dummy inputs / outputs.\n");
	printf("-j [joystick]\tSpecify an alternade joystick (client only; default: /dev/input/js0).\n");
	printf("-f [file]\tSpecify a file or named pipe to read data from (client only).\n");
	printf("-o [file]\tSpecify a shared library file for outputs (robot only).\n");
}

//Catch SIGINT and shut down properly
void signalHandler(int signal) {
	if(signal == SIGINT) {
		printf("SIGINT received!\n");
		running = false;
	}
}
