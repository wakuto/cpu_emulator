CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o\

ALL_CH=cpu.h
HEX=program.hex
CFLAGS=-Wall -pthread -fsanitize=thread

all: $(OBJS) $(ALL_CH)
	$(CC) $(CFLAGS) $^ -o rv32i_emu

prog_asm: 
	riscv32-unknown-elf-gcc -c program.S -o program.o
	riscv32-unknown-elf-ld program.o -o program.elf
	riscv32-unknown-elf-objdump -d program.elf

prog: 
	riscv32-unknown-elf-gcc -mabi=ilp32 -march=rv32i -c program.c -o program.o
	riscv32-unknown-elf-gcc -mabi=ilp32 -march=rv32i -c start.S -o start.o
	riscv32-unknown-elf-ld program.o start.o -lc -L/opt/riscv/riscv32-unknown-elf/lib/ -Tlink.ld -static -o program.elf
	riscv32-unknown-elf-objcopy -O binary program.elf program.bin

run:
	./a.out

clean:
	rm $(OBJS)
