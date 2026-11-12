CC = riscv64-unknown-elf-gcc
CFLAGS = -O2 -Wall -march=rv32imac -mabi=ilp32
LDFLAGS = -nostartfiles

OBJS = startup.o main.o tetris.o display.o menu.o

all: tetris.elf

tetris.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o tetris.elf