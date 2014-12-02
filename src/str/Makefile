# PROG=`basename $(PWD)`
PROG=test.bin

# compiler
CC = g++

# linker
LD = g++

# language file extension
EXT = cpp

# flags
CFLAGS = -O0 -Wall -Wextra -c -g -std=c++11
LDFLAGS = -s

# source and object files
SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)

RM = rm -f

$(PROG): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(PROG)

# meta-rule to generate the object files
%.o: %.$(EXT)
	$(CC) $(CFLAGS) -o $@ $<

# clean rule
clean:
	$(RM) *.o $(PROG)