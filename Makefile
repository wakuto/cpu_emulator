CC=gcc
OBJS=main.o\
	pipeline.o\
  cpu.o

all: $(OBJS)
	$(CC) $^ -o a.out

run:
	./a.out

clean:
	rm $(OBJS)
