//Serial.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <stdexcept>
#include "util/serial.h"

//Constructor
Serial::Serial() : Serial("/dev/ttyUSB0", 9600, false, false, 0, 0) {

}

Serial::Serial(const char *file, int baudRate, bool useParity, bool extraStopBit, unsigned char readMinChars, unsigned char readTimeout) : fd(0),
																																		   tty{0} {
	//Set up serial interface
		//http://linux.die.net/man/3/termios
	fd = open(file, O_RDWR | O_NOCTTY);
	if(fd < 0) {
		printf("Serial connection initialization failed!\n");
		throw std::runtime_error("serial initialization failed");
	}

	//Input/output flags
	tty.c_iflag = 0;			//No input processing
	if(useParity) {
		tty.c_iflag |= INPCK;	//Enable input parity checking
	}
	tty.c_oflag = 0;			//No output processing
	//Control flags
	tty.c_cflag = CSIZE;		//Character size mask
	tty.c_cflag |= CS8;			//8 data bits
	if(useParity) {
		tty.c_cflag &= PARENB;	//1 parity bit
	} else {
		tty.c_cflag &= ~PARENB;	//No parity bits
	}
	if(extraStopBit) {
		tty.c_cflag &= CSTOPB;	//2 stop bits
	} else {
		tty.c_cflag &= ~CSTOPB;	//1 stop bit
	}
	tty.c_cflag |= CREAD;		//Enable read
	tty.c_cflag |= CLOCAL;		//Ignore control lines
	//Local flags
	tty.c_lflag = 0;			//No local processing: use non-canonical (raw) mode, disable echo, etc.
	//Control characters
	tty.c_cc[VMIN] = readMinChars;	//Block on read until specified number of characters have been read
	tty.c_cc[VTIME] = readTimeout;	//Time-based read, or timeout for blocking read

	//Set intial baud rate
	setBaudRate(baudRate, false);

	//Flush serial port and apply settings
	if(tcsetattr(fd, TCSAFLUSH, &tty) != 0) {
		perror("Error applying serial port settings!\n");
		throw std::runtime_error("serial initialization failed");
	}

	printf("Serial connection initialized!\n");
}

//Destructor
Serial::~Serial() {
	close(fd);
}

//Functions
//Set serial baud rate
bool Serial::setBaudRate(int baudRate, bool apply /* = true */) {
	//Set new baud rate
	if(baudRate == 2400) {
		cfsetispeed(&tty, B2400);
		cfsetospeed(&tty, B2400);
	} else if(baudRate == 9600) {
		cfsetispeed(&tty, B9600);
		cfsetospeed(&tty, B9600);
	} else if(baudRate == 38400) {
		cfsetispeed(&tty, B38400);
		cfsetospeed(&tty, B38400);
	} else if(baudRate == 115200) {
		cfsetispeed(&tty, B115200);
		cfsetospeed(&tty, B115200);
	} else {
		return false;
	}

	//Flush serial port and apply settings
	if(apply) {
		return tcsetattr(fd, TCSAFLUSH, &tty) == 0;	//True on success applying settings, false on error
	}

	return true;
}

//Receive serial data
int Serial::serialRead(void *data, int length) {
	return read(fd, data, length);
}

//Transmit serial data
int Serial::serialWrite(void *data, int length) {
	return write(fd, data, length);
}

//Check if new data is available
bool Serial::serialSelect(int timeout) {
	fd_set readSet;
	FD_SET(fd, &readSet);
	struct timeval maxTime;
	maxTime.tv_sec = 0;
	maxTime.tv_usec = timeout;
	return select(fd + 1, &readSet, NULL, NULL, &maxTime) != 1;	//If false, no bytes available; wrong baud rate?
}
