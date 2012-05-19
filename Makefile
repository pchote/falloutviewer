
CC = gcc
CFLAGS = -g -c -Wall -Wno-unknown-pragmas --std=c99
LFLAGS = 

SRC = main.c dat2reader.c
OBJ = $(SRC:.c=.o)

falloutviewer: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LFLAGS)

clean:
	-rm $(OBJ) falloutviewer

.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
