//Server.cpp

//Includes
#include <stdio.h>
#include <stdexcept>
#include <netinet/in.h>
#include "server.h"
#include "telepacket.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"

//Constructor
Server::Server() : Server("0.0.0.0", "8353", true) {

}

Server::Server(const char *address, const char *port, bool listen /* = true */) : unknownAddress{0},
																				  clientAddress{0},
																				  robotAddress{0},
																				  listening(listen),
																				  keepalive(500) {
	//Init socket
	if(listen) {
		s.openSocket(address, port, NULL, NULL);
	} else {
		s.openSocket(NULL, NULL, address, port);
	}
	if(!s.isOpen()) {
		printf("Socket initialization failed!\n");
		throw std::runtime_error("socket initialization failed");
	} else {
		printf("Socket initialized.\n");
	}

	//Don't block on read so the main loop can check for SIGINT
	s.blockRead(false);

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
	s.closeSocket();
}

//Functions
//Main server loop
void Server::run() {
	//Display last received data and send pings once every 500ms
	if(!keepalive.isAlive()) {
		if(in.isClient) {	//Data recieved from client
			printData(in);
		} else {
			printf("Packet %d recieved from robot.\n", in.frameNum);
		}
		sendPing(clientAddress);
		sendPing(robotAddress);
		keepalive.feed();
	}

	//Get data from stream
	in.head = '\0';
	if(s.readData((void *)&in, sizeof(in), &unknownAddress) < 0 || in.head != 'T') {
		return;
	}
	if(in.ping) {
		handlePing();
		return;
	}

	//Send client's data to robot
	s.writeData(&robotAddress, (void *)&in, sizeof(in));
}

//Process a received ping
void Server::handlePing() {
	if(listening) {
		if(in.isClient) {
			printf("Ping %d received from client!\n", in.frameNum);
			clientAddress = unknownAddress;
			sendPing(clientAddress);
		} else {
			printf("Ping %d received from robot!\n", in.frameNum);
			robotAddress = unknownAddress;
			sendPing(robotAddress);
		}
	} else {
		printf("Ping %d recieved!\n", in.frameNum);
	}
}

//Attempt to ping a remote client
void Server::sendPing(struct sockaddr_in &remoteAddress) {
	if(remoteAddress.sin_addr.s_addr != 0) {
		s.writeData(&remoteAddress, (void *)&ping, sizeof(ping));
	}
}

//Print the contents of a data packet to the console
void Server::printData(TelePacket &data) {
	printf("Client frame: %d\n", data.frameNum);
	for(int i = 0; i < TelePacket::NUM_AXES; i++) {
		printf("Axis %d: %f\n", i, data.axes[i]);
	}
	for(int i = 0; i < TelePacket::NUM_BUTTONS; i++) {
		printf("Button %d: %c\n", i, data.buttons[i] ? 'T' : 'F');
	}
}