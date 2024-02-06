CC = gcc
CFLAGS = -Wall --std=c99

OUT_BIN = sfa_exe

all: build

build: sfa.o main.o
	$(CC) -o $(OUT_BIN) main.o sfa.o $(CFLAGS)

sfa.o: sfa.c
	$(CC) -c sfa.c

main.o: main.c
	$(CC) -c main.c

.PHONY: clean

clean:
	rm sfa_exe ./*.o