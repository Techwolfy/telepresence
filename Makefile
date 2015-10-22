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
OBJS=client.o robot.o server.o telepresence.o controlFile.o dummyJoystick.o joystick.o parallax.o pololu.o raspi.o udpsocket.o watchdog.o
ROBOTS=dummyRobot.o basicRobot.o parallaxRobot.o pololuRobot.o raspiRobot.o


#Build everything
.PHONY: all
all: telepresence dummy.so pololu.so raspi.so parallax.so

#Generic object file build message
.PHONY: obj-build
obj-build:
	@echo "Building object files..."

#Load dependency rules
-include $(OBJS:.o=.d)
-include $(ROBOTS:.o=.d)

#Generic compilation rule
%.o: %.cpp obj-build
	$(CXX) -c -o $@ $< $(CFLAGS) -MMD

#Basic robot compilation rules
parallaxRobot.o: basicRobot.cpp obj-build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPARALLAX -MMD

pololuRobot.o: basicRobot.cpp obj-build
	$(CXX) -c -o $@ $< $(CFLAGS) -DPOLOLU -MMD

raspiRobot.o: basicRobot.cpp obj-build
	$(CXX) -c -o $@ $< $(CFLAGS) -DRASPI -MMD

#Build archive of universal files
telepresence.a: $(OBJS) obj-build
	$(AR) telepresence.a $(OBJS)


#Build main binary
telepresence: telepresence.a dummyRobot.o
	@echo "Building telepresence binary..."
	$(CXX) -o $@ $^ $(LIBS)

#Build output module shared libraries
dummy.so: telepresence.a basicRobot.o
	@echo "Building dummy output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

parallax.so: telepresence.a parallaxRobot.o
	@echo "Building parallax output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

pololu.so: telepresence.a pololuRobot.o
	@echo "Building pololu output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(POLOLULIBS)

raspi.so: telepresence.a raspiRobot.o
	@echo "Building raspi output module..."
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(RASPILIBS)


#Clean up old build files
.PHONY: clean
clean:
	@echo "Cleaning up old build files..."
	rm -f *.o *.d *.a *.so telepresence
