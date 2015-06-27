#ifndef BASE_H
#define BASE_H

//Includes
#include "telepacket.h"
#include "udpsocket.h"

class Base {
public:
	//Constructor
	Base() = delete;

	Base(const char *address, const char *port);

	//Destructor
	virtual ~Base();

	//Functions
	virtual void run() = 0;

protected:
	//Variables
	UDPSocket s;
	TelePacket in;
	TelePacket out;
	TelePacket ping;

	//Functions
	virtual void printData(TelePacket &data);
};

#endif //BASE_H
