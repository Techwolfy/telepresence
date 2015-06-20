CC=gcc
CFLAGS=-I. -Ilib -Ilib/pololu/static/include -Ilib/wiringPi/static/include -Llib/pololu/static/lib -Llib/wiringPi/static/lib
LIBS=-lrt -lwiringPi -lRapaPololuMaestro

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

all: %.o
	$(CC)-o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean: rm *.o