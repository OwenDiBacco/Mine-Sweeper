CC = gcc
SRC = src/*.c
OUT = bin/main

CFLAGS = $(shell pkg-config --cflags gtk4)
LIBS   = $(shell pkg-config --libs gtk4)

all:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LIBS)