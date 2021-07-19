CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o\

all: $(OBJS)
	$(CC) $^ -o a.out

prog: 
	riscv32-unknown-elf-gcc -c program.S -o program.o
	riscv32-unknown-elf-ld program.o -o program.elf
	riscv32-unknown-elf-objdump -d program.elf

run:
	./a.out

clean:
	rm $(OBJS)
