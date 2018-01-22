# ECE254 Lab Linux Port
# Makefile: build produce_process, consume and produce_thread

CC=gcc
CFLAGS=-Wall -g
LD=gcc
LDFLAGS=-g -lrt -lm


OBJS1=produce_process.o 
OBJS2=consume.o 
OBJS3=produce_thread.o

all: produce_process consume produce_thread

produce_process: $(OBJS1) 
	$(LD) -o $@ $(OBJS1) $(LDFLAGS) 

consume: $(OBJS2) 
	$(LD) -o $@ $(OBJS2) $(LDFLAGS)

produce_thread: $(OBJS3)
	$(LD) -o $@ $(OBJS3) $(LDFLAGS) -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	$(CC) -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *.d *.o *.out 
