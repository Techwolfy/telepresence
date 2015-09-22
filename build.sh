#!/bin/bash
shopt -s extglob

#Build script for telepresence
#BasicRobot defaults to a dummy motor; use -DPOLOLU for a 6-port Pololu device, -DRASPI for a Raspberry Pi, or -DPARALLAX for a 16-port Parallax USB servo controller.

CC="g++"
AR="ar rsc"
OPTIONS="-std=c++11 -fPIC -ffunction-sections -fdata-sections -Wl,--gc-sections"
SHARED="-fPIC -shared"
INCLUDE="-I. -Iinput/*.h -Ioutput/*.h -Iutil/*.h -Irobot/*.h -Ilib/pololu/static/include -Ilib/wiringPi/static/include"
LIBPATH="-Llib/pololu/static/lib -Llib/wiringPi/static/lib"
LIBS="-ldl -ljsoncpp -lRapaPololuMaestro -lwiringPi"
FILES="*.cpp input/*.cpp output/*.cpp util/*.cpp robot/*.cpp"

#Clean up old build files
rm -f *.o *.a *.so telepresence

#Object and archive files
echo "Building object files..."
${CC} ${OPTIONS} -c ${FILES} ${INCLUDE} $@
${CC} ${OPTIONS} -DPOLOLU -c robot/basicRobot.cpp -o pololuRobot.o ${INCLUDE} $@
${CC} ${OPTIONS} -DRASPI -c robot/basicRobot.cpp -o raspiRobot.o ${INCLUDE} $@
${CC} ${OPTIONS} -DPARALLAX -c robot/basicRobot.cpp -o parallaxRobot.o ${INCLUDE} $@
${AR} telepresence.a !(*Robot*).o

#telepresence
echo "Building telepresence..."
${CC} ${OPTIONS} -o telepresence telepresence.a dummyRobot.o ${LIBPATH} ${LIBS} $@

#dummy.so
echo "Building dummy output module..."
${CC} ${OPTIONS} ${SHARED} -o dummy.so basicRobot.o telepresence.a ${LIBPATH} ${LIBS} $@

#pololu.so
echo "Building pololu output module..."
${CC} ${OPTIONS} ${SHARED} -o pololu.so pololuRobot.o telepresence.a ${LIBPATH} ${LIBS} $@

#raspi.so
echo "Building raspi output module..."
${CC} ${OPTIONS} ${SHARED} -o raspi.so raspiRobot.o telepresence.a ${LIBPATH} ${LIBS} $@

#parallax.so
echo "Building parallax output module..."
${CC} ${OPTIONS} ${SHARED} -o parallax.so parallaxRobot.o telepresence.a ${LIBPATH} ${LIBS} $@

#Clean up object and archive files
rm -f *.o *.a
