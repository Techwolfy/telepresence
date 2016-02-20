#ifndef SERIAL_H
#define SERIAL_H

//Includes
#include <termios.h>

//Declaration
class Serial {
public:
	//Constructor
	Serial();
	Serial(const char *file, int baudRate, bool useParity, bool extraStopBit, unsigned char readMinChars, unsigned char readTimeout);

	//Destructor
	~Serial();

	//Functions
	bool setBaudRate(int baudRate, bool apply = true);
	int serialRead(void *data, int length);
	int serialWrite(void *data, int length);
	bool serialSelect(int timeout);

private:
	//Variables
	int fd;
	struct termios tty;
};

#endif //SERIAL_H
