#!/bin/sh

#telepresenced
g++ -std=c++11 -o telepresenced telepresenced.cpp lib/*.cpp -I. -Ilib/*.h -lrt

#telepresence-pololu
g++ -std=c++11 -DPOLOLU -o telepresence-pololu telepresence.cpp lib/*.cpp mod/*.cpp -I. -Ilib/*.h -Imod/*.h -Ilib/pololu/static/include -Ilib/wiringPi/static/include -Llib/pololu/static/lib -Llib/wiringPi/static/lib -lrt -lwiringPi -lRapaPololuMaestro

#telepresence-raspi
g++ -std=c++11 -DRASPI -o telepresence-raspi telepresence.cpp lib/*.cpp mod/*.cpp -I. -Ilib/*.h -Imod/*.h -Ilib/pololu/static/include -Ilib/wiringPi/static/include -Llib/pololu/static/lib -Llib/wiringPi/static/lib -lrt -lwiringPi -lRapaPololuMaestro
