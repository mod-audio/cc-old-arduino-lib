all: clean
	g++ -Wall -Wextra -c stimer.cpp -o stimer.o
	g++ -Wall -Wextra test.cpp stimer.o -o test

clean:
	rm *.o test