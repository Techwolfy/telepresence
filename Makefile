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
ROBOTS=dummyRobot.o basicRobot.o pololuRobot.o parallaxRobot.o raspiRobot.o


#Load dependency rules
-include $(OBJS:.o=.d)
-include $(ROBOTS:.o=.d)

#Compilation rules
.PHONY: all
all: telepresence dummy.so pololu.so raspi.so parallax.so

#Generic compilation rule
%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -MMD

#Basic robot compilation rules
parallaxRobot.o: basicRobot.cpp
	$(CXX) -c -o $@ $^ $(CFLAGS) -DPARALLAX -MMD

pololuRobot.o: basicRobot.cpp
	$(CXX) -c -o $@ $^ $(CFLAGS) -DPOLOLU -MMD

raspiRobot.o: basicRobot.cpp
	$(CXX) -c -o $@ $^ $(CFLAGS) -DRASPI -MMD

#Build archive of universal files
telepresence.a: $(OBJS)
	$(AR) telepresence.a $(OBJS)


#Build main binary
telepresence: telepresence.a dummyRobot.o
	$(CXX) -o $@ $^ $(LIBS)

#Build output module shared libraries
dummy.so: telepresence.a basicRobot.o
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)

pololu.so: telepresence.a pololuRobot.o
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(POLOLULIBS)

raspi.so: telepresence.a raspiRobot.o
	$(CXX) $(SHARED) -o $@ $^ $(LIBS) $(RASPILIBS)

parallax.so: telepresence.a parallaxRobot.o
	$(CXX) $(SHARED) -o $@ $^ $(LIBS)


#Clean up old build files
.PHONY: clean
clean:
	rm -f *.o *.d *.a *.so telepresence
