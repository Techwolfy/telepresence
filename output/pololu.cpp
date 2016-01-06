//Pololu.cpp

//Includes
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdexcept>
#include "output/pololu.h"

//Constructor
Pololu::Pololu() : Pololu("/dev/ttyACM0") {

}

Pololu::Pololu(const char *file) {
	//Set up Pololu Maestro servo controller interface
	pololuFD = open(file, O_RDWR | O_NOCTTY);
	if(pololuFD < 0) {
		printf("Pololu initialization failed!\n");
		throw std::runtime_error("pololu initialization failed");
	}

	//Serial connection is 115200 8 N 1, full duplex
		//https://www.pololu.com/docs/0J40/5.b

	//Input/output flags
	tty.c_iflag = 0;		//No input processing
	tty.c_oflag = 0;		//No output processing
	//Control flags
	tty.c_cflag = CSIZE;	//Character size mask
	tty.c_cflag |= CS8;		//8 data bits
	tty.c_cflag &= ~PARENB;	//No parity
	tty.c_cflag &= ~CSTOPB;	//1 stop bit
	tty.c_cflag |= CREAD;	//Enable read
	tty.c_cflag |= CLOCAL;	//Ignore control lines
	//Local flags
	tty.c_lflag = 0;		//No local processing: use non-canonical (raw) mode, disable echo, etc.
	//Control characters
	tty.c_cc[VMIN] = 2;		//Block on read until 2 chars have been read
	tty.c_cc[VTIME] = 0;	//Read timeout not used

	//Set intial baud rate
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);

	//Flush serial port and apply settings
	if(tcsetattr(pololuFD, TCSAFLUSH, &tty) != 0) {
		perror("Error applying Pololu Maestro serial port settings");
		throw std::runtime_error("pololu initialization failed");
	}

	printf("Pololu Maestro servo controller initialized!\n");

	//Make sure all motors are stopped
	stop();
}

//Destructor
Pololu::~Pololu() {
	stop();
	close(pololuFD);
}

//Functions
//Set power of all motors
//NB: Supports up to 24-port Pololu Maestro servo controllers; however, smaller controllers will ignore commands for ports they don't have.
void Pololu::control(int numValues, double values[]) {
	for(int i = 0; i < numValues && i < 24; i++) {
		setPower(i, scalePower(values[i]));
	}
}

//Stop all motors
void Pololu::stop() {
	for(int i = 0; i < 24; i++) {
		setPower(i, scalePower(0));
	}
}

//Scale power from (-1.0, 1.0) to Pololu range of (4000, 8000)
unsigned short Pololu::scalePower(double power) {
	if(power > 1.0) {
		power = 1.0;
	} else if(power < -1.0) {
		power = -1.0;
	}

	//Scale from (-1.0, 1.0) to (4000, 8000) quarter-of-microseconds; 0 is 6000 (neutral)
	return (power * 2000) + 6000;
}

//Get power of a specific channel
unsigned short Pololu::getPower(unsigned char channel) {
	if(channel < 0 || channel > 24) {
		printf("Invalid PWM channel!\n");
		return 0;
	}

	unsigned char command[2] = {0x90, channel};
	//Command byte, channel byte
	if(write(pololuFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Pololu Maestro servo controller!\n");
	}

	unsigned char data[2] = {0};
	if(read(pololuFD, &data, sizeof(data)) == sizeof(data)) {
		return (data[0] << 8) | data[1];
	} else {
		printf("Error reading from Pololu Maestro servo controller!\n");
		return 0;
	}
}

//Set power of a specific channel
void Pololu::setPower(unsigned char channel, unsigned short power) {
	if(channel < 0 || channel > 24) {
		printf("Invalid PWM channel!\n");
		return;
	}

	unsigned char command[4] = {0x84, channel, (unsigned char)(power & 0x7F), (unsigned char)((power >> 7) & 0x7F)};
	//Command byte, channel byte, target low 7 bits, target high 7 bits
	if(write(pololuFD, &command, sizeof(command)) != sizeof(command)) {
		printf("Error writing to Pololu Maestro servo controller!\n");
	}
}
