#Telepresence Makefile

#Variables
VPATH=$(wildcard */)
CXX=g++
AR=ar rsc
CFLAGS=-std=c++11 -fPIC -ffunction-sections -fdata-sections -Wl,--gc-sections -I.
LFLAGS=
SHARED=-fPIC -shared
LIBS=-ldl -ljsoncpp
POLOLULIBS=-Llib/pololu/static/lib -lRapaPololuMaestro
RASPILIBS=-Llib/wiringPi/static/lib -lwiringPi
OBJS=client.o robot.o server.o telepresence.o controlFile.o dummyJoystick.o joystick.o dummyMotor.o parallax.o pololu.o raspi.o udpsocket.o watchdog.o
ROBOTS=dummyRobot.o basicRobot.o parallaxRobot.o pololuRobot.o raspiRobot.o
BUILDOBJS=$(addprefix build/, $(OBJS))
BUILDROBOTS=$(addprefix build/, $(ROBOTS))


#Build everything
.PHONY: all
all: bin/telepresence bin/dummy.so bin/pololu.so bin/raspi.so bin/parallax.so

#Load dependency rules
-include $(BUILDOBJS:.o=.d)
-include $(BUILDROBOTS:.o=.d)

#Create folder for intermediate build files
build:
	@echo "Building object files..."
	mkdir -p build

#Generic compilation rule
build/%.o: %.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -MMD

#Basic robot compilation rules
build/parallaxRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPARALLAX -MMD

build/pololuRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPOLOLU -MMD

build/raspiRobot.o: basicRobot.cpp | build
	$(CXX) -c -o $@ $< $(CFLAGS) -DRASPI -MMD

#Build archive of robot-independent files
build/telepresence.a: $(BUILDOBJS) | build
	$(AR) $@ $(BUILDOBJS)


#Create folder for output binaries
bin:
	mkdir -p bin

#Build main binary
bin/telepresence: build/telepresence.a build/dummyRobot.o | bin
	@echo "Building telepresence binary..."
	$(CXX) -o $@ $^ $(LIBS)

#Build output module shared libraries
bin/dummy.so: build/basicRobot.o build/telepresence.a | bin
	@echo "Building dummy output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

bin/parallax.so: build/parallaxRobot.o build/telepresence.a | bin
	@echo "Building parallax output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

bin/pololu.so: build/pololuRobot.o build/telepresence.a | bin
	@echo "Building pololu output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(POLOLULIBS)

bin/raspi.so: build/raspiRobot.o build/telepresence.a | bin
	@echo "Building raspi output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(RASPILIBS)


#Clean up old build files
.PHONY: clean
clean:
	@echo "Cleaning up old build files..."
	rm -rf build/ bin/
