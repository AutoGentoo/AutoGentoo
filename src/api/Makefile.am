AM_CFLAGS = ${AUTOGENTOO_CFLAGS} -I$(top_srcdir)/include/ -Wno-unused-result
ACLOCAL_AMFLAGS='-I m4'

CYTHON_INCLUDES = -I../../include/ -I./venv/include
CYTHON_LD_PATH = -L../.libs/ -L../../hacksaw/src
CYTHON_LIBS = -l:../.libs/libautogentoo.a -l:../../hacksaw/src/libhacksaw.a

mainlibdir = $(libdir)/autogentoo

build_dir:
	mkdir -p build

BUILT_SOURCES = build_dir

build/cython_dynamic_binary.c: cython_dynamic_binary.pyx cython_dynamic_binary.pxd
	cython cython_dynamic_binary.pyx cython_dynamic_binary.pxd -o build/cython_dynamic_binary.c
build/d_malloc.c: d_malloc.pyx d_malloc.pxd
	cython d_malloc.pyx -o build/d_malloc.c
build/cython_vector.c: cython_vector.pyx cython_vector.pxd
	cython cython_vector.pyx -o build/cython_vector.c
#crypt.c: crypt.pyx crypt.pxd
#	cython crypt.pyx
#interface.c: interface.pyx interface.pxd
#	cython interface.pyx
#op_socket.c: op_socket.pyx op_socket.pxd
#	cython op_socket.pyx
#op_string.c: op_string.pyx op_string.pxd
#	cython op_string.pyx
#log.c: log.pyx
#	cython log.pyx
#request.c: request.pyx request.pxd
#	cython request.pyx
#worker.c: worker.pyx worker.pxd
#	cython worker.pyx
#job.c: job.pyx job.pxd
#	cython job.pyx

d_malloc$(PYTHON_EXTENSION_SUFFIX): build/d_malloc.c ../.libs/libautogentoo.a ../../hacksaw/src/libhacksaw.a
	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c build/d_malloc.c -o build/d_malloc.o
	$(CC) $(PYTHON_LIBS) -shared build/d_malloc.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o d_malloc$(PYTHON_EXTENSION_SUFFIX)
cython_dynamic_binary$(PYTHON_EXTENSION_SUFFIX): build/cython_dynamic_binary.c ../.libs/libautogentoo.a ../../hacksaw/src/libhacksaw.a
	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c build/cython_dynamic_binary.c -o build/cython_dynamic_binary.o
	$(CC) $(PYTHON_LIBS) -shared build/cython_dynamic_binary.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o cython_dynamic_binary$(PYTHON_EXTENSION_SUFFIX)
cython_vector$(PYTHON_EXTENSION_SUFFIX): build/cython_vector.c ../.libs/libautogentoo.a ../../hacksaw/src/libhacksaw.a
	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c build/cython_vector.c -o build/cython_vector.o
	$(CC) $(PYTHON_LIBS) -shared build/cython_vector.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o cython_vector$(PYTHON_EXTENSION_SUFFIX)
#crypt$(PYTHON_EXTENSION_SUFFIX): crypt.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c crypt.c -o build/crypt.o
#	$(CC) $(PYTHON_LIBS) -shared build/crypt.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o crypt$(PYTHON_EXTENSION_SUFFIX)
#interface$(PYTHON_EXTENSION_SUFFIX): interface.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c interface.c -o build/interface.o
#	$(CC) $(PYTHON_LIBS) -shared build/interface.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o interface$(PYTHON_EXTENSION_SUFFIX)
#op_socket$(PYTHON_EXTENSION_SUFFIX): op_socket.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c op_socket.c -o build/op_socket.o
#	$(CC) $(PYTHON_LIBS) -shared build/op_socket.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o op_socket$(PYTHON_EXTENSION_SUFFIX)
#op_string$(PYTHON_EXTENSION_SUFFIX): op_string.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c op_string.c -o build/op_string.o
#	$(CC) $(PYTHON_LIBS) -shared build/op_string.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o op_string$(PYTHON_EXTENSION_SUFFIX)
#log$(PYTHON_EXTENSION_SUFFIX): log.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c log.c -o build/log.o
#	$(CC) $(PYTHON_LIBS) -shared build/log.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o log$(PYTHON_EXTENSION_SUFFIX)
#request$(PYTHON_EXTENSION_SUFFIX): request.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c request.c -o build/request.o
#	$(CC) $(PYTHON_LIBS) -shared build/request.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o request$(PYTHON_EXTENSION_SUFFIX)
#worker$(PYTHON_EXTENSION_SUFFIX): worker.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c worker.c -o build/worker.o
#	$(CC) $(PYTHON_LIBS) -shared build/worker.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o worker$(PYTHON_EXTENSION_SUFFIX)
#job$(PYTHON_EXTENSION_SUFFIX): job.c
#	$(CC) -pthread -fPIC $(CYTHON_INCLUDES) $(PYTHON_INCLUDES) -c job.c -o build/job.o
#	$(CC) $(PYTHON_LIBS) -shared build/job.o $(CYTHON_LD_PATH) $(CYTHON_LIBS) -o job$(PYTHON_EXTENSION_SUFFIX)

clean-local:
	rm -rf d_malloc$(PYTHON_EXTENSION_SUFFIX) build/d_malloc.o build/d_malloc.c
	rm -rf interface$(PYTHON_EXTENSION_SUFFIX) build/interface.o interface.c
	rm -rf op_socket$(PYTHON_EXTENSION_SUFFIX) build/op_socket.o op_socket.c
	rm -rf op_string$(PYTHON_EXTENSION_SUFFIX) build/op_string.o op_string.c
	rm -rf crypt$(PYTHON_EXTENSION_SUFFIX) build/crypt.o crypt.c
	rm -rf log$(PYTHON_EXTENSION_SUFFIX) build/log.o log.c
	rm -rf request$(PYTHON_EXTENSION_SUFFIX) build/request.o request.c
	rm -rf worker$(PYTHON_EXTENSION_SUFFIX) build/worker.o worker.c
	rm -rf job$(PYTHON_EXTENSION_SUFFIX) build/job.o job.c
	rm -rf cython_dynamic_binary$(PYTHON_EXTENSION_SUFFIX) build/cython_dynamic_binary.o build/cython_dynamic_binary.c
	rm -rf cython_vector$(PYTHON_EXTENSION_SUFFIX) build/cython_vector.o build/cython_vector.c
	rm -rf build

mainlib_DATA = \
                  d_malloc$(PYTHON_EXTENSION_SUFFIX) \
                  cython_dynamic_binary$(PYTHON_EXTENSION_SUFFIX) \
                  cython_vector$(PYTHON_EXTENSION_SUFFIX)
#                 log$(PYTHON_EXTENSION_SUFFIX) \
#                 op_socket$(PYTHON_EXTENSION_SUFFIX) \
#                 interface$(PYTHON_EXTENSION_SUFFIX) \
#                 op_string$(PYTHON_EXTENSION_SUFFIX) \
#                 crypt$(PYTHON_EXTENSION_SUFFIX) \
#                 request$(PYTHON_EXTENSION_SUFFIX)
#                 worker$(PYTHON_EXTENSION_SUFFIX) \
#                 job$(PYTHON_EXTENSION_SUFFIX)
