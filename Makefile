CC = gcc
CCFLAGS = -std=c99 -g
CCLIBS = -lSDL2

all: main.out

main.out: main.c
	$(CC) $(CCFLAGS) main.c -o main.out $(CCLIBS)

clean:
	rm -fv main.out

run: main.out
	./main.out

.PHONY: clean
