#ifndef SERIAL_H
#define SERIAL_H

//Includes
#ifndef _WIN32	//Linux/POSIX support
	#include <termios.h>
#else			//Windows support
	#include <windows.h>
#endif

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
#ifndef _WIN32
	int fd;
	struct termios tty;
#else
	HANDLE handle;
	DCB dcb;
	COMMTIMEOUTS timeout;
#endif
};

#endif //SERIAL_H
