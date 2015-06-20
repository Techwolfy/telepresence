#!/bin/sh
#Build script for telepresence
#Defaults to dummy joystick; use -DJOYSTICK for a standard joystick.
#Defaults to a dummy motor; use -DPOLOLU for a 6-port Pololu device or -DRASPI for a Raspberry Pi.

CC="g++"
OPTIONS="-std=c++11"
INCLUDE="-I. -Ilib/*.h -Imod/*.h -Ilib/pololu/static/include -Ilib/wiringPi/static/include -Llib/pololu/static/lib -Llib/wiringPi/static/lib"
LIBS="-lrt -lwiringPi -lRapaPololuMaestro"
FILES="telepresence.cpp lib/*.cpp mod/*.cpp"

#telepresenced
${CC} ${OPTIONS} -o telepresenced ${FILES} ${INCLUDE} ${LIBS}

#telepresence-client
${CC} ${OPTIONS} -DJOYSTICK -o telepresence-client ${FILES} ${INCLUDE} ${LIBS}

#telepresence-pololu
${CC} ${OPTIONS} -DPOLOLU -o telepresence-pololu ${FILES} ${INCLUDE} ${LIBS}

#telepresence-raspi
${CC} ${OPTIONS} -DRASPI -o telepresence-raspi ${FILES} ${INCLUDE} ${LIBS}
