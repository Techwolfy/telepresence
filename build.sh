#!/bin/bash
shopt -s extglob

#Build script for telepresence
#BasicRobot defaults to a dummy motor; use -DPOLOLU for a 6-port Pololu device or -DRASPI for a Raspberry Pi.

CC="g++"
AR="ar rsc"
OPTIONS="-std=c++11 -fPIC -ffunction-sections -fdata-sections -Wl,--gc-sections"
SHARED="-fPIC -shared"
INCLUDE="-I. -Iinput/*.h -Ioutput/*.h -Iutil/*.h -Irobot/*.h -Ilib/pololu/static/include -Ilib/wiringPi/static/include"
LIBPATH="-Llib/pololu/static/lib -Llib/wiringPi/static/lib"
LIBS="-lRapaPololuMaestro -ldl -lwiringPi"
FILES="*.cpp input/*.cpp output/*.cpp util/*.cpp robot/*.cpp"

#Object and archive files
echo "Building object files..."
${CC} ${OPTIONS} -c ${FILES} ${INCLUDE} $@
${CC} ${OPTIONS} -DPOLOLU -c robot/basicRobot.cpp -o pololuRobot.o ${INCLUDE} $@
${CC} ${OPTIONS} -DRASPI -c robot/basicRobot.cpp -o raspiRobot.o ${INCLUDE} $@
${AR} telepresence.a !(*Robot*).o

#telepresenced
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

#Clean up object and archive files
rm *.o *.a
