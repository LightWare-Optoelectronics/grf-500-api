#!/bin/bash

mkdir -p ./bin

CFLAGS="-I../ -DLW_DEBUG_LEVEL=1 -O3 -s -Weverything -Wno-declaration-after-statement -Wno-missing-prototypes -Wno-implicit-int-conversion -Wno-padded -Wno-unused-parameter -Wno-unsafe-buffer-usage -Wno-switch-default -Wno-format-nonliteral -Wno-extra-semi-stmt"
SHARED_SOURCES_LINUX="../lw_i2c_api.c ../lw_i2c_api_grf500.c

zig cc -o ./bin/example_basic example_basic.c %SHARED_SOURCES_LINUX% %CFLAGS% -target native-linux -s
