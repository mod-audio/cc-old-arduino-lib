all: clean
	g++ -Wall -Wextra -c actuator.cpp -o actuator.o
	g++ -Wall -Wextra test.cpp actuator.o -o test

clean:
	rm -rf *.o test