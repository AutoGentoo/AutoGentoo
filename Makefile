.PHONY : all
.DEFAULT_GOAL := all

base:
	cd src/base && make

base-test: cglobal-test
	cd src/base && make test

base-clean:
	cd src/base && make clean

cglobal:
	cd src/cglobal && make

cglobal-test:
	cd src/cglobal && make test

cglobal-clean:
	cd src/cglobal && make clean

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
	cd src/cglobal && make
	cd src/base && make
	cd src/http && make
	cd src/portage && make
	cd src/virtual && make

test: cglobal-test http-test portage-test virtual-test

clean: base-clean cglobal-clean http-clean portage-clean virtual-clean
