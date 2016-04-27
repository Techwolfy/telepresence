#Telepresence Makefile

#Makefile settings
SHELL=/bin/bash
GREEN=\e[32m
MAGENTA=\e[35m
CYAN=\e[36m
RESET=\e[0m

#Compilation variables
VPATH=$(wildcard */)
CXX=
AR=
CFLAGS=-std=c++11 -ffunction-sections -fdata-sections -Wl,--gc-sections -Wall -Werror -I. -g
SHARED=-fPIC -shared
LIBS=
OBJS=telepresence.o server.o client.o robot.o udpsocket.o serial.o watchdog.o ratelimit.o log.o dummyJoystick.o joystick.o controlFile.o dummyDevice.o parallax.o pololu.o arduino.o raspi.o
ROBOTS=dummyRobot.o basicRobot.o parallaxRobot.o pololuRobot.o arduinoRobot.o raspiRobot.o redRobot.o quadcopterRobot.o nazaRobot.o
JSONCPPDIR=
ifneq ($(OS), Windows_NT)	#Linux/POSIX support
	CXX+=g++
	AR+=ar
	CFLAGS+=-fPIC
	LIBS+=-ldl -ljsoncpp
else						#Windows support
	CXX+=i686-w64-mingw32-g++
	AR+=i686-w64-mingw32-ar
	CFLAGS+=-Ilib/jsoncpp
	#Compiler bug in mingw-w64 requires this, but the C++11 standard explicitly doesn't
	CFLAGS+=-D__STDC_FORMAT_MACROS
	#MSVCRT stdio functions don't honor C99 format specifiers, so use the MinGW versions
	CFLAGS+=-D__USE_MINGW_ANSI_STDIO=1
	#Require Windows 7 or higher for IPv6 Winsock2 features
	CFLAGS+=-D_WIN32_WINNT=_WIN32_WINNT_WIN7
	#Disable unused Windows features (e.g. Winsock1, which breaks later uses of Winsock2)
	CFLAGS+=-DWIN32_LEAN_AND_MEAN -DNOCRYPT -DNOUSER -DNOGDI -DNOSERVICE -DNOMCX -DNOATOM -DNOGDI -DNOGDICAPMASKS -DNOMETAFILE -DNOMINMAX -DNOMSG -DNOOPENFILE -DNORASTEROPS -DNOSCROLL -DNOSOUND -DNOSYSMETRICS -DNOTEXTMETRIC -DNOWH -DNOCOMM -DNOKANJI -DNOCRYPT -DNOMCX
	LIBS+=-static-libgcc -static-libstdc++ -lws2_32 -lwinmm
	OBJS+=jsoncpp.o
	JSONCPPDIR+=lib/jsoncpp
endif
BUILDOBJS=$(addprefix build/, $(OBJS))
BUILDROBOTS=$(addprefix build/, $(ROBOTS))


#Build everything
.PHONY: all
all: osdetect bin/telepresence bin/dummy.so bin/parallax.so bin/pololu.so bin/arduino.so bin/raspi.so bin/red.so bin/quadcopter.so bin/naza.so

#Print OS version of build
.PHONY: osdetect
osdetect:
ifneq ($(OS), Windows_NT)
	@echo -e "$(MAGENTA)Compiling for Linux.$(RESET)"
else
	@echo -e "$(MAGENTA)Compiling for Windows.$(RESET)"
endif


#Generate libjsoncpp source files
%/jsoncpp/json/json.h %/jsoncpp/jsoncpp.cpp:
	@echo -e "$(CYAN)Building libjsoncpp...$(RESET)"
	git clone https://github.com/open-source-parsers/jsoncpp.git $*
	cd $*; python amalgamate.py
	cd $*; cp -r dist/ jsoncpp/


#Create folder for intermediate build files
build: | osdetect
	@echo -e "$(GREEN)Building object files...$(RESET)"
	mkdir -p build

#Generic compilation rule
build/%.o: %.cpp | build $(if $(filter $(OS), Windows_NT), $(JSONCPPDIR)/jsoncpp/json/json.h)
	$(CXX) -c -o $@ $< $(CFLAGS)

#Libjsoncpp compilation rule
build/jsoncpp.o: $(JSONCPPDIR)/jsoncpp/jsoncpp.cpp $(JSONCPPDIR)/jsoncpp/json/json.h | build
	$(CXX) -c -o $@ $< $(CFLAGS)

#Basic robot compilation rules
build/parallaxRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPARALLAX

build/pololuRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPOLOLU

build/arduinoRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DARDUINO

build/raspiRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DRASPI

#Build archive of robot-independent files
build/telepresence.a: $(BUILDOBJS) | build
	@echo -e "$(CYAN)Creating archive file...$(RESET)"
	$(AR) rsc $@ $^


#Create folder for output binaries
bin: | osdetect
	@echo -e "$(GREEN)Building binary files...$(RESET)"
	mkdir -p bin

#Build main binary
bin/telepresence: build/telepresence.a build/dummyRobot.o | bin
	@echo -e "$(CYAN)Building telepresence binary...$(RESET)"
	$(CXX) -o $@ $^ $(LIBS)

#Build output module shared libraries
bin/%.so: build/%Robot.o build/telepresence.a | bin
	@echo -e "$(CYAN)Building $* output module...$(RESET)"
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

bin/dummy.so: build/basicRobot.o build/telepresence.a | bin
	@echo -e "$(CYAN)Building dummy output module...$(RESET)"
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)


#Clean up old build files
.PHONY: clean
clean:
	@echo -e "$(GREEN)Cleaning up old build files...$(RESET)"
	rm -rf build/ bin/ $(if $(filter $(OS), Windows_NT), $(JSONCPPDIR)/)
