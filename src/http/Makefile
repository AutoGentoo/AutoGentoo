CC = gcc
LL = gcc
INCDIRS = ../
CFLAGS += -Wall -g

BIN = http-server
BARE = server

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.c, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN).o -c $(BIN).c -I$(INCDIRS)
	$(LL) $(OBJECTS) $(BIN).o -o $(BIN)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(BIN).o
	rm -rf *.gch
	rm -rf $(BIN)
