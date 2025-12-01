@REM Compilation for Visual Studio.
@REM Run the Developer Command Prompt for Visual Studio, navigate to this directory and execute this batch file.

set OUT_DIR=bin
set CFLAGS=-nologo -GF -WX -W4 -Gm- -Fo%OUT_DIR%/ -O2 -DLW_DEBUG_LEVEL=1
set INCLUDES=-I..
set SHARED_SOURCES=..\lw_serial_api.c ..\lw_serial_api_grf500.c lw_platform_win_serial.c

if not exist %OUT_DIR% mkdir %OUT_DIR%

cl -Fe%OUT_DIR%/example_basic.exe %CFLAGS% %INCLUDES% %SHARED_SOURCES% example_basic.c
cl -Fe%OUT_DIR%/example_callbacks.exe %CFLAGS% %INCLUDES% %SHARED_SOURCES% example_callbacks.c
cl -Fe%OUT_DIR%/example_unmanaged.exe %CFLAGS% %INCLUDES% %SHARED_SOURCES% example_unmanaged.c

