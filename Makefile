# Define C compiler & flags
CC = gcc
CFLAGS = -Wall -g

# Define libraries to be linked (for example -lm)
LIB = -lm

# Define set of implementation source files and object files
SRC = allocate.c memory.c queue.c data.c hole.c
# OBJ is the same as SRC, just replace .c with .o
OBJ = $(SRC:.c=.o)

# executable target
EXE = allocate

allocate: allocate.o memory.o queue.o data.o hole.o 
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) $(LIB)

allocate.o: allocate.c memory.c queue.h data.h hole.h

queue.o: queue.c queue.h data.h

memory.o: memory.c memory.h data.h hole.h

data.o: data.c data.h hole.h

hole.o: hole.c hole.h

clean:
	rm -f $(OBJ) $(EXE)

format:
	clang-format -i *.c *.h