//Parallax.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdexcept>
#include "output/parallax.h"

//Constructor
Parallax::Parallax() : Parallax("/dev/ttyUSB0", 2400) {

}

Parallax::Parallax(const char *file, unsigned int baud) : parallaxFD(-1),
														  tty{0} {
	//Set up Parallax serial interface
		//http://linux.die.net/man/3/termios
	parallaxFD = open(file, O_RDWR | O_NOCTTY);
	if(parallaxFD < 0) {
		printf("Parallax initialization failed!\n");
		throw std::runtime_error("parallax initialization failed");
	}

	/*//Get current serial port settings
	if(tcgetattr(parallaxFD, &tty) != 0) {
		perror("Error retreiving Parallax serial port settings");
		throw std::runtime_error("parallax initialization failed");
	}

	//Set baud rate
	if(cfsetospeed(&tty, (speed_t)baud) != 0 || cfsetispeed(&tty, (speed_t)baud) != 0) {
		perror("Error setting Parallax serial port baud rate");
		throw std::runtime_error("parallax initialization failed");
	}

	//Other connection settings?
	tty.c_cflag		&=	~PARENB;			// Make 8n1
	tty.c_cflag		&=	~CSTOPB;
	tty.c_cflag		&=	~CSIZE;
	tty.c_cflag		|=	CS8;
	tty.c_cflag		&=	~CRTSCTS;			// no flow control
	tty.c_cc[VMIN]	=	1;					// read doesn't block
	tty.c_cc[VTIME]	=	5;					// 0.5 seconds read timeout
	tty.c_cflag		|=	CREAD;
	tty.c_cflag		|=	CLOCAL;		// turn on READ & ignore ctrl lines

	//Set serial port to raw mode
	cfmakeraw(&tty);

	//Flush serial port and apply settings
	tcflush(parallaxFD, TCIFLUSH);
	if(tcsetattr(parallaxFD, TCSANOW, &tty) != 0) {
		perror("Error applying Parallax serial port settings");
		throw std::runtime_error("parallax initialization failed");
	}*/

	printf("Parallax initialized!\n");

	//Make sure all motors are stopped
	stop();
}

//Destructor
Parallax::~Parallax() {
	stop();
	close(parallaxFD);
}

//Functions
//Set power of all motors
void Parallax::control(int numValues, double values[]) {
	for(int i = 0; i < numValues && i < 16; i++) {
		setPower(i, scalePower(values[i]));
	}
}

//Stop all motors
void Parallax::stop() {
	for(int i = 0; i < 16; i++) {
		setPower(i, scalePower(0.0));
	}
}

//Scale power from (-1.0, 1.0) to Parallax range of (250, 1250)
unsigned short Parallax::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (250, 1250) quarter-of-microseconds; 0 is 750 (neutral)
	return (power * 500) + 750;
}

void Parallax::setPower(unsigned char servoNum, unsigned short power) {
	unsigned char command[8] = {'!', 'S', 'C', servoNum, '\0', '\0', '\0', '\r'};
	command[5] = power;		//Assumes a 16-bit little-endian short; clobbers command[6]
	write(parallaxFD, &command, sizeof(command));
}
