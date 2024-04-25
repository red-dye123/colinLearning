.PHONY: process_copy

INCLUDE_PATH=../include
LIBRARY_PATH=../library
INSTALL_PATH=
SRC_FILE=$(wildcard *.c)
DES_FILE=$(patsubst %.c,%.o,$(SRC_FILE))
CFLAGS=-I$(INCLUDE_PATH) -Wall -g -c
CC=gcc
CPP=g++
CPPFLAGS=
LDFLAGS=
TARGET=process_copy

$(TARGET):$(DES_FILE)
	$(CC) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf $(DES_FILE)
