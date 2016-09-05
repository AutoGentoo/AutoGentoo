CC = gcc
LL = gcc
INCDIRS = ../
CFLAGS += -Wall -g
LIBDIR = ../libraries

BIN = test
PTBIN = profile_test
DTBIN = download_test
DEVTBIN = parted_test

BARE = profile download disk
PTBARE = profile
DTBARE = download
DEVTBARE = disk

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.c, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

PTOBJECTS = $(patsubst %,%.o, $(PTBARE))
PTSOURCES = $(patsubst %,%.c, $(PTBARE))
PTHEADERS = $(patsubst %,%.h, $(PTBARE))

DTOBJECTS = $(patsubst %,%.o, $(DTBARE))
DTSOURCES = $(patsubst %,%.c, $(DTBARE))
DTHEADERS = $(patsubst %,%.h, $(DTBARE))

DEVTOBJECTS = $(patsubst %,%.o, $(DEVTBARE))
DEVTSOURCES = $(patsubst %,%.c, $(DEVTBARE))
DEVTHEADERS = $(patsubst %,%.h, $(DEVTBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PTBIN).o -c $(PTBIN).c -I$(INCDIRS)
	$(LL) $(PTOBJECTS) $(PTBIN).o -o $(PTBIN) -L$(LIBDIR) -lcglobal
	$(CC) $(CFLAGS) -o $(DTBIN).o -c $(DTBIN).c -I$(INCDIRS)
	$(LL) $(DTOBJECTS) $(DTBIN).o -o $(DTBIN) -lcurl
	$(CC) $(CFLAGS) -o $(DEVTBIN).o -c $(DEVTBIN).c -I$(INCDIRS)
	$(LL) $(DEVTOBJECTS) $(DEVTBIN).o -o $(DEVTBIN) -L$(LIBDIR) -lparted -lcglobal
	export LD_LIBRARY_PATH=$(LIBDIR)

install:
	cp $(LIBDIR)/* /usr/local/lib/
	ldconfig

all: $(OBJECTS) test

clean:
	rm -rf $(OBJECTS)
	rm -rf $(PTOBJECTS)
	rm -rf $(DTOBJECTS)
	rm -rf $(BIN)
	rm -rf $(PTBIN)
	rm -rf $(DTBIN)
	rm -rf $(DEVTBIN)
	rm -rf *.o
	rm -rf *.gch
