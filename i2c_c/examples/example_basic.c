// ----------------------------------------------------------------------------
// LightWare I2C API basic example for the GRF-500
// Version: 1.1.0
// Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
// https://www.lightwarelidar.com
// ----------------------------------------------------------------------------
//
// License: MIT No Attribution (MIT-0)
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
// #include <stdint.h>
// #include <stdio.h>
#include <sys/ioctl.h>
// #include <unistd.h>
#include <string.h>

#include "lw_i2c_api_grf500.h"

void lw_debug_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void check_success(lw_result result, const char *error_message) {
    if (result != LW_RESULT_SUCCESS) {
        printf("%s\n", error_message);
        exit(1);
    }
}

// ----------------------------------------------------------------------------
// Platform specific I2C service.
// ----------------------------------------------------------------------------
typedef struct {
    lw_callback_device device;
    uint8_t address;
    int handle;
    // NOTE: Any other custom data can be stored in this context.
} platform_device_context;

lw_result i2c_read_callback(lw_callback_device *device, uint8_t reg,  uint8_t *buffer, uint32_t size) {
    platform_device_context *device_context = (platform_device_context *)device->user_data;

    uint8_t command[] = { reg };

	struct i2c_msg messages[] = {
		{ device_context->address, 0, sizeof(command), command },
		{ device_context->address, I2C_M_RD, size, buffer },
	};

	struct i2c_rdwr_ioctl_data ioctl_data = { messages, 2 };
	int result = ioctl(device_context->handle, I2C_RDWR, &ioctl_data);

	if (result != 2)  {
		return LW_RESULT_ERROR;
	}

    return LW_RESULT_SUCCESS;
}

lw_result i2c_write_callback(lw_callback_device *device, uint8_t reg, uint8_t *buffer, uint32_t size) {
    platform_device_context *device_context = (platform_device_context *)device->user_data;

    uint8_t send_buffer[LW_PACKET_SEND_SIZE];
    memcpy(send_buffer + 1, buffer, size);
	send_buffer[0] = reg;

	struct i2c_msg messages[] = {
    	{ device_context->address, 0, size + 1, send_buffer },
	};

  	struct i2c_rdwr_ioctl_data ioctl_data = { messages, 1 };
  	int result = ioctl(device_context->handle, I2C_RDWR, &ioctl_data);

  	if (result != 1)  {
    	return LW_RESULT_ERROR;
  	}
    
    return LW_RESULT_SUCCESS;
}

lw_result platform_init(platform_device_context *platform_device, const char *bus_device, uint8_t address) {
    platform_device->address = address;
    platform_device->device = lw_create_callback_device(platform_device,
                                                        &i2c_read_callback,
                                                        &i2c_write_callback);

    platform_device->handle = open(bus_device, O_RDWR);

    if (platform_device->handle < 0) {
        return LW_RESULT_ERROR;
    }

    return LW_RESULT_SUCCESS; 
}

// ----------------------------------------------------------------------------
// Application entry point.
// ----------------------------------------------------------------------------
int main(void) {
    // ----------------------------------------------------------------------------
    // Platform related setup.
    // ----------------------------------------------------------------------------
    platform_device_context grf500;

    const char *bus_device = "/dev/i2c-1";
    uint8_t address = 0x66; // Default LightWare I2C address.
    
    check_success(platform_init(&grf500, bus_device, address), "Failed to create I2C device");

    check_success(lw_grf500_initiate_comms(&grf500.device), "Failed to initiate communication\n");

    // ----------------------------------------------------------------------------
    // Get device product info.
    // ----------------------------------------------------------------------------
    lw_grf500_product_info product_info;
    check_success(lw_grf500_get_product_info(&grf500.device, &product_info), "Failed to get product info\n");

    printf("Product name: %s\n", product_info.product_name);
    printf("Hardware version: %u\n", product_info.hardware_version);
    printf("Firmware version: %u.%u.%u\n", product_info.firmware_version.major, product_info.firmware_version.minor, product_info.firmware_version.patch);
    printf("Serial number: %s\n", product_info.serial_number);

    // ----------------------------------------------------------------------------
    // Set up the device.
    // ----------------------------------------------------------------------------
    check_success(lw_grf500_set_update_rate(&grf500.device, 5), "Failed to set update rate\n");

    lw_grf500_distance_config distance_config = LW_GRF500_DISTANCE_CONFIG_ALL;
    check_success(lw_grf500_set_distance_config(&grf500.device, distance_config), "Failed to set distance config\n");

    // ----------------------------------------------------------------------------
    // Poll for distance data.
    // ----------------------------------------------------------------------------
    lw_grf500_distance_data_cm distance_data;
    check_success(lw_grf500_get_distance_data(&grf500.device, distance_config, &distance_data), "Failed to get distance data\n");

    printf("Polled distance: %d cm\n", distance_data.first_return_raw_cm);

    printf("Sample completed\n");

    return 0;
}
