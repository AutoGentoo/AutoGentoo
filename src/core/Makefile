CC = gcc
LL = gcc
INCDIRS = ../
CFLAGS += -Wall -g
LIBDIR = tools/

BIN = test
PTBIN = profile_test
DTBIN = download_test
PTRBIN = ptr_test
#DEVTBIN = devices_test

BARE = profile download ptr_handler
PTBARE = profile
DTBARE = download
PTRBARE = ptr_handler
#DEVTBARE = devices

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.c, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

PTOBJECTS = $(patsubst %,%.o, $(PTBARE))
PTSOURCES = $(patsubst %,%.c, $(PTBARE))
PTHEADERS = $(patsubst %,%.h, $(PTBARE))

PTROBJECTS = $(patsubst %,%.o, $(PTRBARE))
PTRSOURCES = $(patsubst %,%.c, $(PTRBARE))
PTRHEADERS = $(patsubst %,%.h, $(PTRBARE))

DTOBJECTS = $(patsubst %,%.o, $(DTBARE))
DTSOURCES = $(patsubst %,%.c, $(DTBARE))
DTHEADERS = $(patsubst %,%.h, $(DTBARE))

#DEVTOBJECTS = $(patsubst %,%.o, $(DEVTBARE))
#DEVTSOURCES = $(patsubst %,%.c, $(DEVTBARE))
#DEVTHEADERS = $(patsubst %,%.h, $(DEVTBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PTBIN).o -c $(PTBIN).c -I$(INCDIRS)
	$(LL) $(PTOBJECTS) $(PTBIN).o -o $(PTBIN) -L$(LIBDIR) -lcglobal
	$(CC) $(CFLAGS) -o $(DTBIN).o -c $(DTBIN).c -I$(INCDIRS)
	$(LL) $(DTOBJECTS) $(DTBIN).o -o $(DTBIN) -lcurl 
	$(CC) $(CFLAGS) -o $(PTRBIN).o -c $(PTRBIN).c -I$(INCDIRS)
	$(LL) $(PTROBJECTS) $(PTRBIN).o -o $(PTRBIN)
	#$(CC) $(CFLAGS) -o $(DEVTBIN).o -c $(DEVTBIN).c -I$(INCDIRS)
	#$(LL) $(DEVTOBJECTS) $(DEVTBIN).o -o $(DEVTBIN) -L$(LIBDIR) -lcglobal
	$(CC) $(CFLAGS) -o parted_test parted_test.c -lparted

all: $(OBJECTS) test

clean:
	rm -rf $(OBJECTS)
	rm -rf $(PTOBJECTS)
	rm -rf $(DTOBJECTS)
	rm -rf $(BIN)
	rm -rf $(PTBIN)
	rm -rf $(DTBIN)
	rm -rf $(DEVTBIN)
	rm -rf parted_test
	rm -rf $(PTRBIN)
	rm -rf *.o
	rm -rf *.gch
