CC=gcc
OBJS=main.o\
	pipeline.o

all: $(OBJS)
	$(CC) $^

clean:
	rm $(OBJS)
