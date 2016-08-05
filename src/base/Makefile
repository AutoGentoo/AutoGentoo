CC = gcc
LL = gcc
INCDIRS = ../
CFLAGS += -Wall -g
LIB = -lcurl
LIBDIR = ../cglobal/

BIN = test
PTBIN = profile_test
DTBIN = download_test

BARE = profile download
PTBARE = profile
DTBARE = download

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.c, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

PTOBJECTS = $(patsubst %,%.o, $(PTBARE))
PTSOURCES = $(patsubst %,%.c, $(PTBARE))
PTHEADERS = $(patsubst %,%.h, $(PTBARE))

DTOBJECTS = $(patsubst %,%.o, $(DTBARE))
DTSOURCES = $(patsubst %,%.c, $(DTBARE))
DTHEADERS = $(patsubst %,%.h, $(DTBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

test: $(DTOBJECTS)
	#$(CC) $(CFLAGS) -o $(PTBIN).o -c $(PTBIN).c -I$(INCDIRS)
	#$(LL) $(PTOBJECTS) $(PTBIN).o -o $(PTBIN) -L$(LIBDIR) $(LIB)
	$(CC) $(CFLAGS) -o $(DTBIN).o -c $(DTBIN).c -I$(INCDIRS)
	$(LL) $(DTOBJECTS) $(DTBIN).o -o $(DTBIN) -L$(LIBDIR) $(LIB)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(PTOBJECTS)
	rm -rf $(DTOBJECTS)
	rm -rf $(BIN)
	rm -rf $(PTBIN)
	rm -rf $(DTBIN)
	rm -rf *.o
	rm -rf *.gch
