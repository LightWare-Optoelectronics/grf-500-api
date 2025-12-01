#include "lw_platform_linux_serial.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// ----------------------------------------------------------------------------
// Platform specific functions.
// ----------------------------------------------------------------------------
uint32_t convert_baud_rate(uint32_t baud_rate) {
    switch (baud_rate) {
        case 115200: {
            return B115200;
        }
        case 230400: {
            return B230400;
        }
        case 460800: {
            return B460800;
        }
        case 500000: {
            return B500000;
        }
        case 576000: {
            return B576000;
        }
        case 921600: {
            return B921600;
        }
    }

    return B115200;
}

void lw_platform_serial_disconnect(lw_platform_serial_port *serial_port) {
    if (*serial_port >= 0) {
        close(*serial_port);
    }

    *serial_port = -1;
}

lw_result lw_platform_serial_connect(const char *port_name, uint32_t baud_rate, lw_platform_serial_port *serial_port) {
    *serial_port = -1;
    LW_DEBUG_LVL_1("Attempt com connection: %s\n", port_name);

    int32_t descriptor = open(port_name, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);

    if (descriptor < 0) {
        LW_DEBUG_LVL_1("Serial Connect: Failed to open.\n");
        return LW_RESULT_ERROR;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(descriptor, &tty) != 0) {
        LW_DEBUG_LVL_1("Serial Connect: Failed to get attribute.\n");
        return LW_RESULT_ERROR;
    }

    baud_rate = convert_baud_rate(baud_rate);

    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    tty.c_cflag = (tty.c_cflag & ~(tcflag_t)CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~((tcflag_t)PARENB | (tcflag_t)PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag &= ~(tcflag_t)CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_iflag &= ~(tcflag_t)IGNBRK;
    tty.c_iflag &= ~(tcflag_t)ICRNL;
    tty.c_iflag &= ~((tcflag_t)IXON | (tcflag_t)IXOFF | (tcflag_t)IXANY);
    tty.c_lflag = 0;
    tty.c_oflag = 0;

    if (tcsetattr(descriptor, TCSANOW, &tty) != 0) {
        LW_DEBUG_LVL_1("Serial Connect: Failed to set attribute.\n");
        return LW_RESULT_ERROR;
    }

    *serial_port = descriptor;

    LW_DEBUG_LVL_1("Serial Connect: Connected to %s\n", port_name);

    return LW_RESULT_SUCCESS;
}

uint32_t lw_platform_serial_write(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size) {
    if (*serial_port < 0) {
        LW_DEBUG_LVL_1("Serial Write: Invalid Serial Port.\n");
        return 0;
    }

    ssize_t bytes_written = write(*serial_port, buffer, size);

    if (bytes_written != size) {
        return 0;
    }

    return (uint32_t)bytes_written;
}

int32_t lw_platform_serial_read(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size) {
    // NOTE: This function is non-blocking.
    
    if (*serial_port < 0) {
        LW_DEBUG_LVL_1("Serial Write: Invalid Serial Port.\n");
        return 0;
    }

    errno = 0;
    ssize_t bytes_read = read(*serial_port, buffer, size);

    if (bytes_read == -1) {
        if (errno == EAGAIN) {
            return 0;
        }

        return -1;
    }

    return (int32_t)bytes_read;
}

uint32_t lw_platform_get_time_ms(void) {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    int64_t microsecond = time.tv_sec * 1000000 + time.tv_nsec / 1000;
    return (uint32_t)(microsecond / 1000);
}

void lw_platform_sleep(uint32_t time_ms) {
    usleep(time_ms * 1000);
}

// ----------------------------------------------------------------------------
// Device service callbacks.
// ----------------------------------------------------------------------------
uint32_t lw_platform_get_time_ms_callback(lw_callback_device *device) {
    (void)device;
    return lw_platform_get_time_ms();
}

void lw_platform_sleep_callback(lw_callback_device *device, uint32_t time_ms) {
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
    return LW_RESULT_SUCCESS;
}

lw_platform_serial_port lw_platform_create_serial_port(void) {
    return -1;
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
