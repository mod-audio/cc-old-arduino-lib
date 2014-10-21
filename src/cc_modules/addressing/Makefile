all: clean
	g++ -Wall -Wextra -g -c str.cpp -o str.o
	g++ -Wall -Wextra -g -c mode.cpp -o mode.o
	g++ -Wall -Wextra -g -c addressing.cpp -o addressing.o
	g++ -Wall -Wextra -g -c scalepoint.cpp -o scalepoint.o
	g++ -Wall -Wextra test.cpp addressing.o str.o mode.o scalepoint.o -o test

clean:
	rm -rf *.o test