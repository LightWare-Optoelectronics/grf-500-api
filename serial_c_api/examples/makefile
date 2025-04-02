CC=gcc
CFLAGS=-I../ -DLW_DEBUG_LEVEL=1 -O3
SHARED_SOURCES=../lw_serial_api.c ../lw_serial_api_grf500.c lw_platform_linux_serial.c

makeall: example_basic.c example_callbacks.c example_unmanaged.c $(SHARED_SOURCES)
	mkdir -p bin
	gcc -o bin/example_basic example_basic.c $(SHARED_SOURCES) $(CFLAGS)
	gcc -o bin/example_callbacks example_callbacks.c $(SHARED_SOURCES) $(CFLAGS)
	gcc -o bin/example_unmanaged example_unmanaged.c $(SHARED_SOURCES) $(CFLAGS)

