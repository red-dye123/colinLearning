.PHONY:epoll_thread_server

CC=g++ -std=c++11
INCLUDE=../include/
LIBRARY=../library/
INSTAL=
CFLAGS=-I$(INCLUDE) -g -Wall -c
LDFLAGS=-lpthread
TARGET=epoll_thread_server
SOURCE_CODE=$(wildcard *.c)
DEST_CODE=$(patsubst %.c,%.o,$(SOURCE_CODE))
RM=rm -rf

$(TARGET):$(DEST_CODE)
	$(CC) $^ $(LDFLAGS) -o $@
%.o:%.c
	$(CC) $(CFLAGS) $<

clean:
	$(RM) $(DEST_CODE)
