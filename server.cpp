//Server.cpp

//Includes
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <chrono>
#include <string>
#include <stdexcept>
#ifndef _WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
#else
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
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
																								   unknownAddressLength(0),
																								   clientAddressLength(0),
																								   robotAddressLength(0),
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
	if(s.blockRead(false) < 0) {
		printf("Failed to set socket as nonblocking!\n");
		throw std::runtime_error("socket initialization failed");
	}

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
		ping["time"] = (Json::Value::UInt64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		sendPing(&clientAddress, clientAddressLength);
		sendPing(&robotAddress, robotAddressLength);
		ping["frameNum"] = ping.get("frameNum", 0).asUInt() + 1;
		keepalive.feed();
	}

	//Get data from stream
	buffer[0] = '\0';
	unknownAddressLength = sizeof(unknownAddress);
	if(s.readData((void *)&buffer, sizeof(buffer), &unknownAddress, &unknownAddressLength) < 0 || buffer[0] == '\0') {
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
	s.writeData(&robotAddress, robotAddressLength, (void *)buffer, sizeof(buffer));
}

//Process a received ping
void Server::handlePing() {
	if(listening) {
		if(in.get("isClient", true).asBool()) {
			printf("Ping %d received from client!\n", in.get("frameNum", 0).asUInt());
			clientAddress = unknownAddress;
			clientAddressLength = unknownAddressLength;
		} else {
			printf("Ping %d received from robot!\n", in.get("frameNum", 0).asUInt());
			robotAddress = unknownAddress;
			robotAddressLength = unknownAddressLength;
		}
	} else {
		printf("Ping %d recieved!\n", in.get("frameNum", 0).asUInt());
	}
	printLatency(in);
}

//Attempt to ping a remote client
void Server::sendPing(struct sockaddr_storage *remoteAddress, socklen_t remoteAddressLength) {
	if((remoteAddress->ss_family == AF_INET && ((struct sockaddr_in *)remoteAddress)->sin_addr.s_addr != 0) || (remoteAddress->ss_family == AF_INET6 && ((struct sockaddr_in6 *)remoteAddress)->sin6_addr.s6_addr != 0)) {
		std::string pingJSON = writer.write(ping);
		s.writeData(remoteAddress, remoteAddressLength, (void *)pingJSON.c_str(), pingJSON.length());
	}
}

//Check if a given data packet's key is valid
bool Server::validateKey(Json::Value &data) {
	return strcmp(password, data.get("key", "").asCString()) == 0;
}

//Print the contents of a data packet to the console
void Server::printData(Json::Value &data) {
	printf("Client frame: %u\n", data.get("frameNum", 0).asUInt());
	for(unsigned int i = 0; i < data["axes"].size(); i++) {
		printf("Axis %d: %f\n", i, data["axes"].get(i, 0.0).asDouble());
	}
	for(unsigned int i = 0; i < data["buttons"].size(); i++) {
		printf("Button %d: %c\n", i, data["buttons"].get(i, false).asBool() ? 'T' : 'F');
	}
}

//Print the latency of a data packet to the console
void Server::printLatency(Json::Value &data) {
	printf("Sent (ms): %" PRIu64 ", Received (ms): %" PRIu64 ", Latency (ms): %" PRIu64 "\n", data.get("time", 0).asUInt64(), (Json::Value::UInt64)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - data.get("time", 0).asUInt64());
}
