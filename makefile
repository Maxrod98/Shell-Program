# makefile

all: driver

driver: driver.cpp io.o
	g++ -g -w -std=c++11 io.o driver.cpp -o driver

io.o: io.h io.cpp
	g++ -g -w -std=c++11 io.h io.cpp -c io.cpp

clean:
	rm -rf *.o driver

run:
	make all
	./driver
