CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o\

CFLAGS=-Wall -pthread -fsanitize=thread

all: $(OBJS)
	$(CC) $(CFLAGS) $^ -o rv32i_emu

prog_asm: 
	riscv32-unknown-elf-gcc -c program.S -o program.o
	riscv32-unknown-elf-ld program.o -o program.elf
	riscv32-unknown-elf-objdump -d program.elf

prog: 
	riscv32-unknown-elf-gcc -c program.c -o program.o
	riscv32-unknown-elf-gcc -c start.S -o start.o
	riscv32-unknown-elf-ld program.o start.o -lc -L/opt/riscv/riscv32-unknown-elf/lib/ -Tlink.ld -static -o program.elf
	riscv32-unknown-elf-objcopy -O binary program.elf program.bin
	hexdump -v -e '1/4 "%08x" "\n"' program.bin | sed 's/^/0x/' | sed 's/$$/,/' > program.hex
	riscv32-unknown-elf-readelf -h program.elf | grep "Entry point address"


run:
	./a.out

clean:
	rm $(OBJS)
