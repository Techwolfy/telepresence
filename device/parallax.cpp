//Parallax.cpp

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdexcept>
#include <sys/select.h>
#include "device/parallax.h"

//Constructor
Parallax::Parallax() : Parallax("/dev/ttyUSB0") {

}

Parallax::Parallax(const char *file) : parallaxFD(-1),
									   tty{0} {

	//Set up Parallax servo controller serial interface
		//http://linux.die.net/man/3/termios
	parallaxFD = open(file, O_RDWR | O_NOCTTY);
	if(parallaxFD < 0) {
		printf("Parallax servo controller initialization failed!\n");
		throw std::runtime_error("parallax initialization failed");
	}

	//Serial connection is 2400 8 N 2, half duplex, with remote echo enabled:
		//https://media.digikey.com/pdf/Data%20Sheets/Parallax%20PDFs/28823.pdf
		//http://forums.parallax.com/discussion/comment/610370/#Comment_610370

	//Input/output flags
	tty.c_iflag = 0;		//No input processing
	tty.c_oflag = 0;		//No output processing
	//Control flags
	tty.c_cflag = CSIZE;	//Character size mask
	tty.c_cflag |= CS8;		//8 data bits
	tty.c_cflag &= ~PARENB;	//No parity
	tty.c_cflag |= CSTOPB;	//2 stop bits
	tty.c_cflag |= CREAD;	//Enable read
	tty.c_cflag |= CLOCAL;	//Ignore control lines
	//Local flags
	tty.c_lflag = 0;		//No local processing: use non-canonical (raw) mode, disable echo, etc.
	//Control characters
	tty.c_cc[VMIN] = 8;		//Block on read until 8 chars have been read or read() request is satisfied
	tty.c_cc[VTIME] = 1;	//1 decisecond read timeout (NB: All functions which receive 3-byte responses trigger this)

	//Set intial baud rate
	cfsetispeed(&tty, B2400);
	cfsetospeed(&tty, B2400);

	//Flush serial port and apply settings
	if(tcsetattr(parallaxFD, TCSAFLUSH, &tty) != 0) {
		perror("Error applying Parallax servo controller serial port settings");
		throw std::runtime_error("parallax initialization failed");
	}

	try {
		setBaudRate(true);
	} catch(std::runtime_error &err) {
		printf("Communication with Parallax servo controller failed, trying other baud rate...\n");
		setBaudRate(false);
		setBaudRate(true);
	}

	printf("Parallax initialized! Firmware Version: %.1f\n", getVersion());

	//Make sure all motors are stopped
	stopMotors();
}

//Destructor
Parallax::~Parallax() {
	stopMotors();
	close(parallaxFD);
}

//Functions
//Increase device-side connection speed from 2400 baud to 38400 baud
void Parallax::setBaudRate(bool increase) {
	unsigned char command[8] = {'!','S', 'C', 'S', 'B', 'R', increase, '\r'};
	//3 preamble bytes, 3 command bytes, baud rate byte, command terminator
	if(write(parallaxFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Parallax servo controller!\n");
	}

	//Remove remote echo from input buffer, ignore any errors
	read(parallaxFD, &command, sizeof(command));

	//Set local baud rate
	if(increase) {
		cfsetispeed(&tty, B38400);
		cfsetospeed(&tty, B38400);
	} else {
		cfsetispeed(&tty, B2400);
		cfsetospeed(&tty, B2400);
	}

	//Flush serial port and apply settings
	if(tcsetattr(parallaxFD, TCSAFLUSH, &tty) != 0) {
		perror("Error setting Parallax serial port baud rate");
		throw std::runtime_error("parallax speed increase failed");
	}

	//Check if a response was received
	fd_set readSet;
	FD_SET(parallaxFD, &readSet);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;	//1 decisecond
	if(select(parallaxFD + 1, &readSet, NULL, NULL, &timeout) != 1) {
		//No response received; wrong baud rate?
		printf("Error communicating with Parallax servo controller!\n");
		throw std::runtime_error("parallax speed increase failed");
	}

	//Verify new baud rate
	unsigned char data[3] = {0};
	//Response is "BR#", where # is the device's current baud rate setting (0x00 or 0x01)
	//FIXME: First character of response is sometimes dropped on baud rate increase, so check the second byte if the third is missing
	if((unsigned int)read(parallaxFD, &data, sizeof(data)) <= (ssize_t)sizeof(data) - 1) {
		printf("Error reading from Parallax servo controller!\n");
	}
	if(data[2] == increase || data[1] == increase) {
		printf("Parallax serial connection baud rate changed successfully!\n");
	} else {
		printf("Error setting Parallax servo controller baud rate!\n");
		throw std::runtime_error("parallax speed increase failed");
	}
}

//Set power of a specific motor
void Parallax::setMotorPower(unsigned char channel, double power) {
	setPower(channel, scalePower(power));
}

//Stop all motors
void Parallax::stopMotors() {
	for(int i = 0; i < PARALLAX_NUM_MOTORS; i++) {
		setPower(i, scalePower(0.0));
	}
}

//Get firmware version of Parallax servo controller
double Parallax::getVersion() {
	unsigned char command[8] = {'!', 'S', 'C', 'V', 'E', 'R', '?', '\r'};
	//3 preamble bytes, 4 command bytes, command terminator
	if(write(parallaxFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Parallax servo controller!\n");
	}
	read(parallaxFD, &command, sizeof(command));	//Remove remote echo from input buffer, ignore any errors

	unsigned char data[4] = {0};
	//Response is a 3-character string, so add a zero terminator
	if(read(parallaxFD, &data, sizeof(data) - 1) != sizeof(data) - 1) {
		printf("Error reading from Parallax servo controller!\n");
	}
	return atof((const char *)&data);
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

//Get power of a specific channel
unsigned short Parallax::getPower(unsigned char channel) {
	if(channel > PARALLAX_NUM_MOTORS) {
		printf("Invalid PWM channel!\n");
		return 0;
	}

	unsigned char command[8] = {'!', 'S', 'C', 'R', 'S', 'P', channel, '\r'};
	//3 preamble bytes, 3 command bytes, channel byte, command terminator
	if(write(parallaxFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Parallax servo controller!\n");
	}
	read(parallaxFD, &command, sizeof(command));	//Remove remote echo from input buffer, ignore any errors

	unsigned char data[3] = {0};
	if(read(parallaxFD, &data, sizeof(data)) == sizeof(data)) {
		return (data[1] << 8) | data[2];
	} else {
		return 0;
	}
}

//Set power of a specific channel
void Parallax::setPower(unsigned char channel, unsigned short power) {
	if(channel > PARALLAX_NUM_MOTORS) {
		printf("Invalid PWM channel!\n");
		return;
	}

	unsigned char command[8] = {'!', 'S', 'C', channel, '\0', (unsigned char)(power & 0xFF), (unsigned char)((power >> 8) & 0xFF), '\r'};
	//3 preamble byte, channel byte, ramp byte, power low byte, power high byte, command terminator
	if(write(parallaxFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Parallax servo controller!\n");
	}
	read(parallaxFD, &command, sizeof(command));	//Remove remote echo from input buffer, ignore any errors
}