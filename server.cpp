//Server.cpp

//Includes
#include <stdio.h>
#include <netinet/in.h>
#include "server.h"
#include "telepacket.h"
#include "udpsocket.h"

//Constructor
Server::Server() : Server("0.0.0.0", "8353") {

}

Server::Server(const char *address, const char *port) : Base(address, port),
														unknownAddress{0},
														clientAddress{0},
														robotAddress{0} {
	//Set up output packet
	out.frameNum = 0;
	out.isClient = false;
	out.isRobot = false;
	out.ping = false;

	//Set up ping response packet
	ping.frameNum = 0;
	ping.isClient = false;
	ping.isRobot = false;
	ping.ping = true;
}

//Destructor
Server::~Server() {

}

//Functions
//Main server loop
void Server::run() {
	//Get data from stream
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in), &unknownAddress) < 0 || in.head != 'T') {
		return;
	}
	if(in.ping) {
		ping.frameNum = in.frameNum;
		if(in.isClient) {
			printf("Ping %d received from client!\n", in.frameNum);
			clientAddress = unknownAddress;
			sendPing(clientAddress);
		} else {
			printf("Ping %d received from robot!\n", in.frameNum);
			robotAddress = unknownAddress;
			sendPing(robotAddress);
		}
		return;
	}

	if(in.isClient) {	//Data recieved from client
		printData(in);
	} else {
		printf("Packet %d recieved from robot.\n", in.frameNum);
	}

	//Send client's data to robot
	s.writeData(&robotAddress, (void *)&in, sizeof(in));
}

//Attempt to ping both the client and the robot
void Server::sendPing(struct sockaddr_in &remoteAddress) {
	if(remoteAddress.sin_addr.s_addr != 0) {
		s.writeData(&remoteAddress, (void *)&ping, sizeof(ping));
	}
}
