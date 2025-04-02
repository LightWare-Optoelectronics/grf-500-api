#ifndef LW_PLATFORM_WINDOWS_H
#define LW_PLATFORM_WINDOWS_H

#include "lw_serial_api.h"

#include <stdlib.h>

#ifdef cplusplus
extern "C" {
#endif

typedef int32_t lw_platform_serial_port;

typedef struct {
    lw_callback_device device;
    lw_platform_serial_port serial_port;
} lw_platform_serial_device;

lw_result lw_platform_create_serial_device(const char *port_name, uint32_t baud_rate, lw_platform_serial_device *platform_device);

lw_result lw_platform_init(void);
uint32_t lw_platform_get_time_ms(void);
void lw_platform_sleep(uint32_t time_ms);

lw_platform_serial_port lw_platform_create_serial_port(void);
lw_result lw_platform_serial_connect(const char *port_name, uint32_t baud_rate, lw_platform_serial_port *serial_port);
void lw_platform_serial_disconnect(lw_platform_serial_port *serial_port);
uint32_t lw_platform_serial_write(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size);
int32_t lw_platform_serial_read(lw_platform_serial_port *serial_port, uint8_t *buffer, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif // LW_WINDOWS_PLATFORM_H
