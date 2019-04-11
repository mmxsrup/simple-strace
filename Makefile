CC = gcc
CFLAGS = -Wall -std=c11
RM = rm
PYTHON = python3

OBJS = strace.o

all: $(OBJS) gen_h
	$(CC) $(CFLAGS) -o strace $(OBJS)

gen_h:
	$(RM) -f syscall_table.h
	$(PYTHON) gen_syscall_table.py

clean:
	$(RM) -f strace *.o syscall_table.h