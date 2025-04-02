if not exist "./bin" mkdir "./bin"

set CFLAGS=-I../ -DLW_DEBUG_LEVEL=1 -O3 -s -Weverything -Wno-declaration-after-statement -Wno-missing-prototypes -Wno-implicit-int-conversion -Wno-padded -Wno-unused-parameter -Wno-unsafe-buffer-usage -Wno-switch-default -Wno-format-nonliteral -Wno-extra-semi-stmt
set SHARED_SOURCES_WIN=../lw_serial_api.c ../lw_serial_api_grf500.c lw_platform_win_serial.c
set SHARED_SOURCES_LINUX=../lw_serial_api.c ../lw_serial_api_grf500.c lw_platform_linux_serial.c

zig cc -o ./bin/example_basic.exe example_basic.c %SHARED_SOURCES_WIN% %CFLAGS% -target native-windows -s
zig cc -o ./bin/example_callbacks.exe example_callbacks.c %SHARED_SOURCES_WIN% %CFLAGS% -target native-windows -s
zig cc -o ./bin/example_unmanaged.exe example_unmanaged.c %SHARED_SOURCES_WIN% %CFLAGS% -target native-windows -s

zig cc -o ./bin/example_basic example_basic.c %SHARED_SOURCES_LINUX% %CFLAGS% -target native-linux -s
zig cc -o ./bin/example_callbacks example_callbacks.c %SHARED_SOURCES_LINUX% %CFLAGS% -target native-linux -s
zig cc -o ./bin/example_unmanaged example_unmanaged.c %SHARED_SOURCES_LINUX% %CFLAGS% -target native-linux -s

