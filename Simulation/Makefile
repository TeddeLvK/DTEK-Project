CC = gcc
CFLAGS = -O2 -Wall
LDFLAGS =

OBJS = simulator_main.o tetris.o

all: tetris_sim

tetris_sim: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o tetris_sim