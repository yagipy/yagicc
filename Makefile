CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

yagicc: $(OBJS)
	$(CC) -o yagicc $(OBJS) $(LDFLAGS)

$(OBJS): yagicc.h

test: yagicc
	./test.sh

clean:
	rm -f yagicc out* *.o

format:
	clang-format -i *.c *.h

.PHONY: test clean format
