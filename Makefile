CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o\

all: $(OBJS)
	$(CC) $^ -o a.out

prog_asm: 
	riscv32-unknown-elf-gcc -c program.S -o program.o
	riscv32-unknown-elf-ld program.o -o program.elf
	riscv32-unknown-elf-objdump -d program.elf

prog_c: 
	riscv32-unknown-elf-gcc -c program.c -o program.o
	riscv32-unknown-elf-gcc -c start.S -o start.o
	riscv32-unknown-elf-ld program.o start.o -lc -L/opt/riscv/riscv32-unknown-elf/lib/ -Tlink.ld -static -o program.elf
	riscv32-unknown-elf-objdump -d program.elf

run:
	./a.out

clean:
	rm $(OBJS)
