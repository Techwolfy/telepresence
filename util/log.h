#ifndef LOG_H
#define LOG_H

//Declaration
class Log {
public:
	//Constructor
	Log() = delete;

	//Destructor
	~Log() = default;

	//Enums
	enum loglevel_t {ERR, WARN, INFO, DEBUG, RAW};

	//Functions
	static void log(loglevel_t level, const char *message);
	static void logf(loglevel_t level, const char *messageFormat, ...);
	static void setLevel(loglevel_t level);
	static void enableStderr(bool enable);

private:
	//Variables
	static loglevel_t loglevel;
	static bool useStdErr;

	//Functions
	static const char* getLevelText(loglevel_t level);
};

#endif //LOG_H
