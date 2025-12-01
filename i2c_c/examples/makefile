CC=gcc
CFLAGS=-I../ -DLW_DEBUG_LEVEL=1 -O3
SHARED_SOURCES=../lw_i2c_api.c ../lw_i2c_api_grf500.c

makeall: example_basic.c $(SHARED_SOURCES)
	mkdir -p bin
	gcc -o bin/example_basic example_basic.c $(SHARED_SOURCES) $(CFLAGS)

