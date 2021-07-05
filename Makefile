CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o\

all: $(OBJS)
	$(CC) $^ -o a.out

asm: asm.o
	$(CC) $^ -o asm.out

run:
	./a.out

clean:
	rm $(OBJS)
