CXX = g++
LXX = g++
INCDIRS = .
CFLAGS += -Wall -g -lz -lbz2  -larchive

BIN = emerge
TBIN = testing
BARE = mstring package map
TBARE = package_test

MOBJECTS = $(patsubst %,%.o, $(BARE))
MSOURCES = $(patsubst %,%.c, $(BARE))
MHEADERS = $(patsubst %,%.h, $(BARE))

TOBJECTS = $(patsubst %,%.o, $(TBARE) $(BARE))
TSOURCES = $(patsubst %,%.c, $(TBARE) $(BARE))
THEADERS = $(patsubst %,%.h, $(TBARE) $(BARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.cpp %.h
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)


all: $(MOBJECTS)
	$(eval INCDIRS=.)
	$(LXX) $(MOBJECTS) -o $(BIN)

test: $(TOBJECTS)
	$(eval INCDIRS=test)
	$(LXX) $(TOBJECTS) -o $(TBIN)

clean:
	rm -rf $(MOBJECTS)
	rm -rf $(TOBJECTS)
	rm -rf $(BIN)
	rm -rf $(TBIN)
