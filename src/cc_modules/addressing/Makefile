all: clean
	g++ -Wall -Wextra -c str.cpp -o str.o
	g++ -Wall -Wextra -c mode.cpp -o mode.o
	g++ -Wall -Wextra -c addressing.cpp -o addressing.o
	g++ -Wall -Wextra test.cpp addressing.o str.o mode.o -o test

clean:
	rm -rf *.o test