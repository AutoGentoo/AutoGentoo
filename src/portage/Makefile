CXX = g++
LXX = g++
INCDIRS = .
CFLAGS += -Wall -g

BIN = emerge
BARE = bash_config ebuild file _misc_tools parse_config version blocks emerge findPackage packageConfig type warning config emergepackage is_dir package use option

OBJECTS = $(patsubst %,%.o, $(BARE))
SOURCES = $(patsubst %,%.cpp, $(BARE))
HEADERS = $(patsubst %,%.hh, $(BARE))

.PHONY : all
.DEFAULT_GOAL := all

%.o : %.cpp %.h
	$(CXX) $(CFLAGS) -o $@ -c $< -I$(INCDIRS)


all: $(OBJECTS)
	$(eval INCDIRS=.)
	$(CXX) $(CFLAGS) -o emerge-bin.o -c emerge-bin.cpp -I$(INCDIRS)
	$(LXX) $(OBJECTS) emerge-bin.o -o $(BIN)

clean:
	rm -rf $(OBJECTS)
	rm -rf emerge-bin.o
	rm -rf *.gch
	rm -rf $(BIN)
