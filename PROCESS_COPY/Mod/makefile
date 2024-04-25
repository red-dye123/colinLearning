.PHONY: copy

INCLUDE_PATH=
LIBRARY_PATH=../library
INSTALL_PATH=
SRC_FILE=$(wildcard *.c)
DES_FILE=$(patsubst %.c,%.o,$(SRC_FILE))
CFLAGS=-I$(INCLUDE_PATH) -Wall -g -c
CC=gcc
CPP=g++
CPPFLAGS=
LDFLAGS=
TARGET=copy

$(TARGET):$(DES_FILE)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf $(DES_FILE)
