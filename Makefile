CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

tinyc: $(OBJS)
	$(CC) -o tinyc $(OBJS) $(LDFLAGS)

$(OBJS): tinyc.h

test: tinyc
	./test.sh

clean:
	rm -f tinyc out* *.o

format:
	clang-format -i *.c *.h

.PHONY: test clean format
