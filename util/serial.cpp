//Serial.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
	#include <termios.h>
	#include <sys/select.h>
#else
	#include <windows.h>
#endif
#include <stdexcept>
#include "util/serial.h"

//Constructor
#ifndef _WIN32
Serial::Serial() : Serial("/dev/ttyUSB0", 9600, false, false, 0, 0) {
#else
Serial::Serial() : Serial("\\\\.\\COM1", 9600, false, false, 0, 0) {
#endif

}

#ifndef _WIN32
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
		tty.c_cflag &= PARENB;	//Enable output parity generation and input parity checking
	} else {
		tty.c_cflag &= ~PARENB;	//Disable parity generation
	}
	if(extraStopBit) {
		tty.c_cflag &= CSTOPB;	//2 stop bits
	} else {
		tty.c_cflag &= ~CSTOPB;	//1 stop bit
	}
	tty.c_cflag |= CREAD;		//Enable read
	tty.c_cflag |= CLOCAL;		//Ignore control lines
	tty.c_cflag &= ~PARODD;		//Even parity, if used
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
#else
Serial::Serial(const char *file, int baudRate, bool useParity, bool extraStopBit, unsigned char readMinChars, unsigned char readTimeout) : handle(0),
																																		   dcb{0},
																																		   timeout{0} {
	//Set up serial interface
		//https://msdn.microsoft.com/en-us/library/ms810467.aspx
	handle = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(handle < 0) {
		printf("Error opening serial port: %lu\n", GetLastError());
		throw std::runtime_error("serial initialization failed");
	}

	//Set basic serial port settings (baud rate, parity, number of data bits, and number of stop bits)
	dcb.DCBlength = sizeof(dcb);
	dcb.BaudRate = baudRate;
	dcb.fBinary = 1;
	dcb.fParity = useParity;
	dcb.ByteSize = 8;
	if(useParity) {
		dcb.Parity = EVENPARITY;
	} else {
		dcb.Parity = NOPARITY;
	}
	if(extraStopBit) {
		dcb.StopBits = TWOSTOPBITS;
	} else {
		dcb.StopBits = ONESTOPBIT;
	}
	dcb.ErrorChar = '\0';
	if(!SetCommState(handle, &dcb)) {
		printf("Error applying serial port settings: %lu\n", GetLastError());
		throw std::runtime_error("serial initialization failed");
	}

	//Set read timeouts
	if(readTimeout > 0) {
		if(readMinChars > 0) {	//Intercharacter timeout
			timeout.ReadIntervalTimeout = readTimeout * 100;	//Deciseconds to milliseconds
		} else {				//Total timeout
			timeout.ReadTotalTimeoutConstant = readTimeout * 100;	//Deciseconds to milliseconds
		}
	} else {
		if(readMinChars > 0) {	//Counted read
			//Handled by length parameter of serialRead(); timeouts remain 0 (unused)
		} else {				//"Nonblocking" read
			timeout.ReadTotalTimeoutConstant = 1;	//Wait as little as possible for a blocking read (1 millisecond)
		}
	}
	if(!SetCommTimeouts(handle, &timeout)) {
		printf("Error setting serial port timeout: %lu\n", GetLastError());
		throw std::runtime_error("serial initialization failed");
	}

	printf("Serial connection initialized!\n");
}
#endif

//Destructor
Serial::~Serial() {
#ifndef _WIN32
	close(fd);
#else
	CloseHandle(handle);
#endif
}

//Functions
//Set serial baud rate
bool Serial::setBaudRate(int baudRate, bool apply /* = true */) {
	//Set new baud rate
#ifndef _WIN32
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
#else
	dcb.BaudRate = baudRate;
#endif

	//Flush serial port and apply settings
	if(apply) {
#ifndef _WIN32
		return tcsetattr(fd, TCSAFLUSH, &tty) == 0;	//True on success applying settings, false on error
#else
		return SetCommState(handle, &dcb);
#endif
	}

	return true;
}

//Receive serial data
int Serial::serialRead(void *data, int length) {
#ifndef _WIN32
	return read(fd, data, length);
#else
	unsigned long readBytes = 0;
	if(!ReadFile(handle, data, length, &readBytes, NULL)) {
		return -1;
	}
	return readBytes;
#endif
}

//Transmit serial data
int Serial::serialWrite(void *data, int length) {
#ifndef _WIN32
	return write(fd, data, length);
#else
	unsigned long writtenBytes = 0;
	if(!WriteFile(handle, data, length, &writtenBytes, NULL)) {
		return -1;
	}
	return writtenBytes;
#endif
}

//Check if new data is available
bool Serial::serialSelect(int timeout) {
#ifndef _WIN32
	fd_set readSet;
	FD_SET(fd, &readSet);
	struct timeval maxTime;
	maxTime.tv_sec = 0;
	maxTime.tv_usec = timeout;
	return select(fd + 1, &readSet, NULL, NULL, &maxTime) == 1;	//If false, no bytes available; wrong baud rate?
#else
	COMSTAT status = {0};
	ClearCommError(handle, NULL, &status);
	if(status.cbInQue > 0) {
		return true;
	} else {
		usleep(timeout);
		ClearCommError(handle, NULL, &status);
		return status.cbInQue > 0;
	}
#endif
}
