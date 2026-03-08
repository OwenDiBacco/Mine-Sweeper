CC = gcc
SRC = src/*.c lib/cjson/cjson.c
OUT = bin/main

CFLAGS = $(shell pkg-config --cflags gtk4)
LIBS   = $(shell pkg-config --libs gtk4) -lcurl

all:
	$(CC) $(SRC) $(CFLAGS) -o $(OUT) $(LIBS)