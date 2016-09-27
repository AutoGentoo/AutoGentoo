.PHONY : all
.DEFAULT_GOAL := all

tools:
	cd core/tools && make

core: tools
	cd core/tools && make

core-test:
	cd core/tools && make test

core-clean:
	cd core/tools && make clean

http:
	cd http && make 

http-test:
	cd http && make test

http-clean:
	cd http && make clean

portage:
	cd portage && make

portage-test:
	cd portage && make test

portage-clean:
	cd portage && make clean

virtual:
	cd virtual && make

virtual-test:
	cd virtual && make test

virtual-clean:
	cd virtual && make clean

all:
	cd core/tools && make
	cd core && make
	cd http && make
	cd portage && make
	cd virtual && make

test: core-test http-test portage-test virtual-test

clean: core-clean http-clean portage-clean virtual-clean
