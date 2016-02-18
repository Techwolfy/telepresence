//Server.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <jsoncpp/json/json.h>
#include "server.h"
#include "util/udpsocket.h"
#include "util/watchdog.h"

//Constructor
Server::Server() : Server("0.0.0.0", "8353", "", true) {

}

Server::Server(const char *address, const char *port, const char *key, bool listen /* = true */) : buffer{0},
																								   unknownAddress{0},
																								   clientAddress{0},
																								   robotAddress{0},
																								   password(key),
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

	//Limit socket receive buffer to prevent excessive lag
	if(s.setRecieveBufferLength(25) < 0) {	//Max delay of 500ms at 50 frames per second
		printf("Failed to limit socket recieve buffer; if control loops run slowly, lag may build up over time.\n");
	}

	//Set up output packet
	out["frameNum"] = 0;
	out["isClient"] = false;
	out["isRobot"] = false;
	out["ping"] = false;
	out["key"] = key;
	out["time"] = 0;

	//Set up ping response packet
	ping["frameNum"] = 0;
	ping["isClient"] = false;
	ping["isRobot"] = false;
	ping["ping"] = true;
	ping["key"] = key;
	ping["time"] = 0;
}

//Destructor
Server::~Server() {
	s.closeSocket();
}

//Functions
//Main server loop
void Server::run() {
	//Display last received data and send pings once every 500ms
	//Don't display pings, just wait for the next loop
	if(!keepalive.isAlive() && !in.get("ping", false).asBool()) {
		if(in.get("isClient", true).asBool()) {	//Data recieved from client
			printData(in);
		} else {
			printf("Packet %d recieved from robot.\n", in.get("frameNum", 0).asUInt());
		}
		ping["time"] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
		sendPing(clientAddress);
		sendPing(robotAddress);
		ping["frameNum"] = ping.get("frameNum", 0).asUInt() + 1;
		keepalive.feed();
	}

	//Get data from stream
	buffer[0] = '\0';
	if(s.readData((void *)&buffer, sizeof(buffer), &unknownAddress) < 0 || buffer[0] == '\0') {
		return;
	} else {
		reader.parse(buffer, in, false);
		if(!validateKey(in)) {
			//Ignore packets with invalid keys
			return;
		}
	}
	if(in.get("ping", false).asBool()) {
		handlePing();
		return;
	}

	//Send client's data to robot
	s.writeData(&robotAddress, (void *)buffer, sizeof(buffer));
}

//Process a received ping
void Server::handlePing() {
	if(listening) {
		if(in.get("isClient", true).asBool()) {
			printf("Ping %d received from client!\n", in.get("frameNum", 0).asUInt());
			clientAddress = unknownAddress;
		} else {
			printf("Ping %d received from robot!\n", in.get("frameNum", 0).asUInt());
			robotAddress = unknownAddress;
		}
	} else {
		printf("Ping %d recieved!\n", in.get("frameNum", 0).asUInt());
	}
	printLatency(in);
}

//Attempt to ping a remote client
void Server::sendPing(struct sockaddr_in &remoteAddress) {
	if(remoteAddress.sin_addr.s_addr != 0) {
		std::string pingJSON = writer.write(ping);
		s.writeData(&remoteAddress, (void *)pingJSON.c_str(), pingJSON.length());
	}
}

//Check if a given data packet's key is valid
bool Server::validateKey(Json::Value &data) {
	return (strcmp(password, in.get("key", "").asCString()) == 0);
}

//Print the contents of a data packet to the console
void Server::printData(Json::Value &data) {
	printf("Client frame: %d\n", data.get("frameNum", 0).asUInt());
	for(unsigned int i = 0; i < data["axes"].size(); i++) {
		printf("Axis %d: %f\n", i, data["axes"].get(i, 0.0).asDouble());
	}
	for(unsigned int i = 0; i < data["buttons"].size(); i++) {
		printf("Button %d: %c\n", i, data["buttons"].get(i, false).asBool() ? 'T' : 'F');
	}
}

//Print the latency of a data packet to the console
void Server::printLatency(Json::Value &data) {
		printf("Sent (ms): %lld, Received (ms): %lld, Latency (ms): %lld\n", data.get("time", 0).asUInt64(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - data.get("time", 0).asUInt64());
}
