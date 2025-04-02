#include "lw_platform_win_serial.h"

static int64_t time_frequency;
static int64_t time_counter_start;

// ----------------------------------------------------------------------------
// Platform specific functions.
// ----------------------------------------------------------------------------
void lw_platform_serial_disconnect(lw_platform_serial_port *serial_port) {
    if (*serial_port != INVALID_HANDLE_VALUE)
        CloseHandle(*serial_port);

    *serial_port = INVALID_HANDLE_VALUE;
}

lw_result lw_platform_serial_connect(const char *port_name, uint32_t baud_rate, lw_platform_serial_port *serial_port) {
    *serial_port = INVALID_HANDLE_VALUE;
    LW_DEBUG_LVL_1("Attempt com connection: %s\n", port_name);

    HANDLE handle = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

    if (handle == INVALID_HANDLE_VALUE) {
        LW_DEBUG_LVL_1("Serial Connect: Failed to open.\n");
        return LW_RESULT_ERROR;
    }

    PurgeComm(handle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);

    DCB comParams = {0};
    comParams.DCBlength = sizeof(comParams);
    GetCommState(handle, &comParams);
    comParams.BaudRate = baud_rate;
    comParams.ByteSize = 8;
    comParams.StopBits = ONESTOPBIT;
    comParams.Parity = NOPARITY;
    comParams.fDtrControl = DTR_CONTROL_ENABLE;
    comParams.fRtsControl = DTR_CONTROL_ENABLE;

    BOOL status = SetCommState(handle, &comParams);

    if (status == FALSE) {
        // NOTE: Some USB<->Serial drivers require the state to be set twice.
        status = SetCommState(handle, &comParams);

        if (status == FALSE) {
            CloseHandle(serial_port);
            return LW_RESULT_ERROR;
        }
    }

    COMMTIMEOUTS timeouts = {0};
    GetCommTimeouts(handle, &timeouts);
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(handle, &timeouts);

    *serial_port = handle;

    LW_DEBUG_LVL_1("Serial Connect: Connected to %s\n", port_name);

    return LW_RESULT_SUCCESS;
}

uint32_t lw_platform_serial_write(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size) {
    if (*serial_port == INVALID_HANDLE_VALUE) {
        LW_DEBUG_LVL_1("Serial Write: Invalid Serial Port.\n");
        return 0;
    }

    OVERLAPPED overlapped = {0};
    DWORD bytesWritten = 0;

    if (!WriteFile(*serial_port, buffer, size, &bytesWritten, &overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            LW_DEBUG_LVL_1("Serial Write: Write Failed.\n");
            return 0;
        } else {
            if (!GetOverlappedResult(*serial_port, &overlapped, &bytesWritten, TRUE)) {
                LW_DEBUG_LVL_1("Serial Write: Waiting Error.\n");
                return 0;
            } else {
                if (bytesWritten != size) {
                    LW_DEBUG_LVL_1("Wrote %d bytes instead of %d.\n", bytesWritten, size);
                    return 0;
                }

                return bytesWritten;
            }
        }
    } else {
        if (bytesWritten != size) {
            LW_DEBUG_LVL_1("Wrote %d bytes instead of %d.\n", bytesWritten, size);
            return 0;
        }

        return bytesWritten;
    }
}

int32_t lw_platform_serial_read(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size) {
    // NOTE: This function is non-blocking.
    
    if (*serial_port == INVALID_HANDLE_VALUE) {
        LW_DEBUG_LVL_1("Serial Read: Invalid Serial Port.\n");
        return 0;
    }

    OVERLAPPED overlapped = {0};
    DWORD bytesRead;

    if (!ReadFile(*serial_port, buffer, size, &bytesRead, &overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            LW_DEBUG_LVL_1("Serial Read: IO Pending Error.\n");
            return -1;
        } else {
            if (!GetOverlappedResult(*serial_port, &overlapped, &bytesRead, TRUE)) {
                LW_DEBUG_LVL_1("Serial Read: Waiting Error: %d.\n", GetLastError());
                return -1;
            } else if (bytesRead > 0) {
                return (int32_t)bytesRead;
            }
        }
    } else if (bytesRead > 0) {
        return (int32_t)bytesRead;
    }

    return 0;
}

uint32_t lw_platform_get_time_ms(void) {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    int64_t time = counter.QuadPart - time_counter_start;
    double result = (double)time / ((double)time_frequency);

    return (uint32_t)(result * 1000);
}

void lw_platform_sleep(uint32_t time_ms) {
    Sleep(time_ms);
}

// ----------------------------------------------------------------------------
// Device service callbacks.
// ----------------------------------------------------------------------------
uint32_t lw_platform_get_time_ms_callback(lw_callback_device *device) {
    (void)device;
    return lw_platform_get_time_ms();
}

void lw_platform_sleep_callback(lw_callback_device *device, uint32_t time_ms) {
    (void)device;
    lw_platform_sleep(time_ms);
}

uint32_t lw_platform_serial_send_callback(lw_callback_device *device, uint8_t *buffer, uint32_t size) {
    lw_platform_serial_device *platform_device = (lw_platform_serial_device *)device->user_data;
    return lw_platform_serial_write(&platform_device->serial_port, buffer, size);
}

int32_t lw_platform_serial_receive_callback(lw_callback_device *device, uint8_t *buffer, uint32_t size, uint32_t timeout_ms) {
    lw_platform_serial_device *platform_device = (lw_platform_serial_device *)device->user_data;
    (void)timeout_ms;
    return lw_platform_serial_read(&platform_device->serial_port, buffer, size);
}

// ----------------------------------------------------------------------------
// Platform context creation.
// ----------------------------------------------------------------------------
lw_result lw_platform_init(void) {
    LARGE_INTEGER freq;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    time_frequency = freq.QuadPart;
    time_counter_start = counter.QuadPart;

    return LW_RESULT_SUCCESS;
}

lw_platform_serial_port lw_platform_create_serial_port(void) {
    return INVALID_HANDLE_VALUE;
}

lw_result lw_platform_create_serial_device(const char *port_name, uint32_t baud_rate, lw_platform_serial_device *platform_device) {
    if (lw_platform_init() != LW_RESULT_SUCCESS) {
        return LW_RESULT_ERROR;
    }

    platform_device->serial_port = lw_platform_create_serial_port();

    if (lw_platform_serial_connect(port_name, baud_rate, &platform_device->serial_port) != LW_RESULT_SUCCESS) {
        return LW_RESULT_ERROR;
    }

    platform_device->device = lw_create_callback_device(platform_device,
                                                        &lw_platform_sleep_callback,
                                                        &lw_platform_get_time_ms_callback,
                                                        &lw_platform_serial_send_callback,
                                                        &lw_platform_serial_receive_callback);

    return LW_RESULT_SUCCESS;
}
