CC = gcc
CFLAGS = -Wall -std=c11
RM = rm

OBJS = strace.o

all: $(OBJS)
	$(CC) $(CFLAGS) -o strace $(OBJS)

clean:
	$(RM) -f strace *.o