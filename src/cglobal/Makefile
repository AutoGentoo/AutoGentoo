CC = gcc
LL = gcc
INCDIRS = .
CFLAGS += -Wall -g
LIB = -lcrypto

ETBIN = encrypt_test
FTBIN = file_test
CTBIN = config_test
ETBARE = encrypt mstring command_tools file fgetln
FTBARE = mstring file fgetln
CTBARE = mstring file configparser fgetln
BARE = mstring encrypt configparser command_tools file fgetln

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.c, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

ETOBJECTS = $(patsubst %,%.o, $(ETBARE))
ETSOURCES = $(patsubst %,%.c, $(ETBARE))
ETHEADERS = $(patsubst %,%.h, $(ETBARE))

FTOBJECTS = $(patsubst %,%.o, $(FTBARE))
FTSOURCES = $(patsubst %,%.c, $(FTBARE))
FTHEADERS = $(patsubst %,%.h, $(FTBARE))

CTOBJECTS = $(patsubst %,%.o, $(CTBARE))
CTSOURCES = $(patsubst %,%.c, $(CTBARE))
CTHEADERS = $(patsubst %,%.h, $(CTBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

test: $(ETOBJECTS) $(FTOBJECTS) $(CTOBJECTS)
	$(eval INCDIRS=.)
	$(CC) $(CFLAGS) -o encrypt_test.o -c encrypt_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o file_test.o -c file_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o config_test.o -c config_test.c -I$(INCDIRS)
	$(LL) $(ETOBJECTS) encrypt_test.o -o $(ETBIN) $(LIB)
	$(LL) $(FTOBJECTS) file_test.o -o $(FTBIN) $(LIB)
	$(LL) $(CTOBJECTS) config_test.o -o $(CTBIN) $(LIB)

all: $(OBJECTS)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(ETOBJECTS)
	rm -rf $(FTOBJECTS)
	rm -rf $(CTOBJECTS)
	rm -rf $(BIN)
	rm -rf $(ETBIN)
	rm -rf $(FTBIN)
	rm -rf $(CTBIN)
	rm -rf *.o
	rm -rf *.gch
