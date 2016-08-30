CC = gcc
LL = gcc
INCDIRS = .
CFLAGS += -Wall -g
LIB = -lcrypto

ETBIN = encrypt_test
FTBIN = file_test
CTBIN = config_test
CTBIN = ptr_test
ETBARE = encrypt mstring command_tools file fgetln ptr_handler
FTBARE = mstring file fgetln ptr_handler
CTBARE = mstring file iniparser dictionary fgetln ptr_handler
PTRBARE = ptr_handler
BARE = mstring encrypt iniparser dictionary command_tools file ptr_handler fgetln

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

PTROBJECTS = $(patsubst %,%.o, $(PTRBARE))
PTRSOURCES = $(patsubst %,%.c, $(PTRBARE))
PTRHEADERS = $(patsubst %,%.h, $(PTRBARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.c %.h
	$(CC) $(CFLAGS) -fPIC -o $@ -c $< -I$(INCDIRS)

test: $(OBJECTS)
	$(CC) $(CFLAGS) -o encrypt_test.o -c encrypt_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o file_test.o -c file_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o config_test.o -c config_test.c -I$(INCDIRS)
	$(CC) $(CFLAGS) -o ptr_test.o -c ptr_test.c -I$(INCDIRS)
	$(LL) $(ETOBJECTS) encrypt_test.o -o $(ETBIN) $(LIB)
	$(LL) $(FTOBJECTS) file_test.o -o $(FTBIN) $(LIB)
	$(LL) $(CTOBJECTS) config_test.o -o $(CTBIN) $(LIB)
	$(LL) $(PTROBJECTS) ptr_test.o -o $(CTBIN) $(LIB)

all: $(OBJECTS)
	gcc -shared -o libcglobal.so $(OBJECTS)
	cp libcglobal.so ../../libraries

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
	rm -rf *.so
