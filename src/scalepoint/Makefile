all: clean
	g++ -Wall -Wextra -c str.cpp -o str.o
	g++ -Wall -Wextra -c scalepoint.cpp -o scalepoint.o
	g++ -Wall -Wextra test.cpp scalepoint.o str.o -o test

clean:
	rm -rf *.o test