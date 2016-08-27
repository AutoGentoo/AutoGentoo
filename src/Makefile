.PHONY : all
.DEFAULT_GOAL := all

base:
	cd base && make

base-test: cglobal-test
	cd base && make test

base-clean:
	cd base && make clean

cglobal:
	cd cglobal && make

cglobal-test:
	cd cglobal && make test

cglobal-clean:
	cd cglobal && make clean

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
	cd cglobal && make
	cd base && make
	cd http && make
	cd portage && make
	cd virtual && make

test: cglobal-test http-test portage-test virtual-test

clean: base-clean cglobal-clean http-clean portage-clean virtual-clean
