CC = gcc
LL = gcc
INCDIRS = .
CFLAGS += -Wall -g
LIB = -lcrypto

ETBIN = encrypt_test
FTBIN = file_test
ETBARE = encrypt mstring command_tools file
FTBARE = mstring file
BARE = mstring encrypt configparer command_tools file

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.cpp, $(BARE))
HEADERS = $(patsubst %,%.h, $(BARE))

ETOBJECTS = $(patsubst %,%.o, $(ETBARE))
ETSOURCES = $(patsubst %,%.cpp, $(ETBARE))
ETHEADERS = $(patsubst %,%.h, $(ETBARE))

FTOBJECTS = $(patsubst %,%.o, $(FTBARE))
FTSOURCES = $(patsubst %,%.cpp, $(FTBARE))
FTHEADERS = $(patsubst %,%.h, $(FTBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)

test: $(ETOBJECTS) $(FTOBJECTS)
	$(eval INCDIRS=.)
	$(CC) $(CFLAGS) -o encrypt_test.o -c encrypt_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o file_test.o -c file_test.c -I$(INCDIRS)
	$(LL) $(ETOBJECTS) encrypt_test.o -o $(ETBIN) $(LIB)
	$(LL) $(FTOBJECTS) file_test.o -o $(FTBIN) $(LIB)

#all: $(OBJECTS)
#	

clean:
	rm -rf $(OBJECTS)
	rm -rf $(ETOBJECTS)
	rm -rf $(FTOBJECTS)
	rm -rf $(BIN)
	rm -rf $(ETBIN)
	rm -rf $(FTBIN)
