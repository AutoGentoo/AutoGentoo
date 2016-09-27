.PHONY : all
.DEFAULT_GOAL := all

tools:
	cd src/core/tools && make

core: tools
	cd src/core && make

core-test:
	cd src/core && make test

core-clean:
	cd src/core && make clean

http:
	cd src/http && make 

http-test:
	cd src/http && make test

http-clean:
	cd src/http && make clean

portage:
	cd src/portage && make

portage-test:
	cd src/portage && make test

portage-clean:
	cd src/portage && make clean

virtual:
	cd src/virtual && make

virtual-test:
	cd src/virtual && make test

virtual-clean:
	cd src/virtual && make clean

all:
	cd core/tools && make
	cd src/core && make
	cd src/base && make
	cd src/http && make
	cd src/portage && make
	cd src/virtual && make

test: core-test http-test portage-test virtual-test

clean: core-clean http-clean portage-clean virtual-clean
