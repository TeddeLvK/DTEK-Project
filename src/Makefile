CC = riscv32-unknown-elf-gcc
CFLAGS = -march=rv32imc -mabi=ilp32 -ffreestanding -O2 -Wall
LDFLAGS = -nostartfiles -Wl,-T,link.ld

OBJS = main.o display.o tetris.o startup.o

all: tetris.elf

tetris.elf: $(OBJS)$(CC) $(CFLAGS)$(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC)$(CFLAGS) -c $< -o$@

%.o: %.S
	$(CC)$(CFLAGS) -c $< -o$@

clean:
	rm -f *.o *.elf