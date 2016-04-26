//Log.cpp

//Includes
#include <stdio.h>
#include <stdarg.h>
#include "util/log.h"

//ANSI Color Codes
#define ANSI_COLOR_BLACK	"\x1b[30m"
#define ANSI_COLOR_RED		"\x1b[31m"
#define ANSI_COLOR_GREEN	"\x1b[32m"
#define ANSI_COLOR_YELLOW	"\x1b[33m"
#define ANSI_COLOR_BLUE		"\x1b[34m"
#define ANSI_COLOR_MAGENTA	"\x1b[35m"
#define ANSI_COLOR_CYAN		"\x1b[36m"
#define ANSI_COLOR_WHITE	"\x1b[37m"
#define ANSI_COLOR_RESET	"\x1b[0m"

//Variables
Log::loglevel_t Log::loglevel = INFO;
bool Log::useStdErr = false;

//Functions
//Retrieve text indicating the message level to precede the log output
const char* Log::getLevelText(loglevel_t level) {
	switch(level) {
		case ERR:
			return "[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "]\t";
			break;
		case WARN:
			return "[" ANSI_COLOR_YELLOW "WARN" ANSI_COLOR_RESET "]\t";
			break;
		case INFO:
			return "[" ANSI_COLOR_GREEN "INFO" ANSI_COLOR_RESET "]\t";
			break;
		case DEBUG:
			return "[" ANSI_COLOR_CYAN "DEBUG" ANSI_COLOR_RESET "]\t";
			break;
		case RAW:
		default:
			return "";
			break;
	}
}

//Write a message to the log
void Log::log(loglevel_t level, const char *message) {
	if(level > loglevel) {
		//Message is more verbose than current log setting, so ignore it.
		return;
	}

	//Log the message
	if(useStdErr && level == ERR) {
		fprintf(stderr, "%s%s\n", getLevelText(level), message);
	} else {
		printf("%s%s\n", getLevelText(level), message);
	}
}

//Write a formatted message to the log
void Log::Log::logf(loglevel_t level, const char *messageFormat, ...) {
	if(level > loglevel) {
		//Message is more verbose than current log setting, so ignore it.
		return;
	}

	//Load variable argument list
	va_list messageArgs;
	va_start(messageArgs, messageFormat);

	//Log the message
	if(useStdErr && level == ERR) {
		fprintf(stderr, "%s", getLevelText(level));
		vfprintf(stderr, messageFormat, messageArgs);
	} else {
		printf("%s", getLevelText(level));
		vprintf(messageFormat, messageArgs);
	}

	//Unload variable argument list
	va_end(messageArgs);
}

//Change the current log level
void Log::setLevel(loglevel_t level) {
	loglevel = level;
}

//Toggle the usage of stderr for errors
void Log::enableStderr(bool enable) {
	useStdErr = enable;
}
