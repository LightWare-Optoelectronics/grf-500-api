// ----------------------------------------------------------------------------
// LightWare Serial API basic example for the GRF-500
// Version: 1.1.0
// Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
// https://www.lightwarelidar.com
// ----------------------------------------------------------------------------
//
// License: MIT No Attribution (MIT-0)
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

#include "lw_serial_api_grf500.h"

#ifdef _WIN32
#include "lw_platform_win_serial.h"
#elif __linux__
#include "lw_platform_linux_serial.h"
#endif

void lw_debug_print(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void check_success(lw_result result, const char *error_message) {
	if (result != LW_RESULT_SUCCESS) {
		printf("%s\\n", error_message);
		exit(1);
	}
}


int main(void) {
    // ----------------------------------------------------------------------------
// Platform related setup.
// ----------------------------------------------------------------------------
lw_platform_serial_device grf500;
// Example Windows COM port: "\\\\\\\\.\\\\COM4"
// Example Linux device serial port: "/dev/ttyACM0"
check_success(lw_platform_create_serial_device("\\\\\\\\.\\\\COM4", 115200, &grf500), "Failed to create serial device");

check_success(lw_grf500_initiate_serial(&grf500.device), "Failed to initiate serial\\n");

// ----------------------------------------------------------------------------
// Get device product info.
// ----------------------------------------------------------------------------
lw_grf500_product_info product_info;
check_success(lw_grf500_get_product_info(&grf500.device, &product_info), "Failed to get product info\\n");

printf("Product name: %s\\n", product_info.product_name);
printf("Hardware version: %d\\n", product_info.hardware_version);
printf("Firmware version: %d.%d.%d\\n", product_info.firmware_version.major, product_info.firmware_version.minor, product_info.firmware_version.patch);
printf("Serial number: %s\\n", product_info.serial_number);

    // ----------------------------------------------------------------------------
// Set up the device.
// ----------------------------------------------------------------------------
check_success(lw_grf500_set_stream(&grf500.device, LW_GRF500_STREAM_ID_NONE), "Failed to set stream: none\\n");
check_success(lw_grf500_set_update_rate(&grf500.device, 5), "Failed to set update rate\\n");

lw_grf500_distance_config distance_config = LW_GRF500_DISTANCE_CONFIG_ALL;
check_success(lw_grf500_set_distance_config(&grf500.device, distance_config), "Failed to set distance config\\n");

// ----------------------------------------------------------------------------
// Poll for distance data.
// ----------------------------------------------------------------------------
lw_grf500_distance_data_cm distance_data;
check_success(lw_grf500_get_distance_data(&grf500.device, distance_config, &distance_data), "Failed to get distance data\\n");

printf("Polled distance: %d cm\\n", distance_data.first_return_raw_cm);

// ----------------------------------------------------------------------------
// Stream distance data: Blocking version.
// ----------------------------------------------------------------------------
check_success(lw_grf500_set_stream(&grf500.device, LW_GRF500_STREAM_ID_DISTANCE), "Failed to set stream: distance\\n");

for (int i = 0; i < 10; ++i) {
	lw_result result = lw_grf500_wait_for_streamed_distance(&grf500.device, distance_config, &distance_data, 1000);

	if (result == LW_RESULT_SUCCESS) {
		printf("Streamed distance: %d cm\\n", distance_data.first_return_raw_cm);
	} else if (result == LW_RESULT_TIMEOUT) {
		// TODO: Could be timeout or communication error.
		printf("Stream timeout\\n");
	} else if (result == LW_RESULT_ERROR) {
		printf("Communication error\\n");
		return 1;
	}
}

// ----------------------------------------------------------------------------
// Stream distance data: Non-blocking version.
// ----------------------------------------------------------------------------
for (int i = 0; i < 10; ++i) {
	while (1) {
		printf("Attempting to get response...\\n");
		// NOTE: The timeout is set to 0.
		lw_result result = lw_grf500_wait_for_streamed_distance(&grf500.device, distance_config, &distance_data, 0);

		if (result == LW_RESULT_SUCCESS) {
			printf("Non blocking streamed distance: %d cm\\n", distance_data.first_return_raw_cm);
			break;
		} else if (result == LW_RESULT_AGAIN) {
			printf("Full response not received yet, waiting/doing other work...\\n");
			lw_platform_sleep(50);
			continue;
		} else {
			printf("Communication error\\n");
			return 1;
		}
	}
}

// ----------------------------------------------------------------------------
// Closing down.
// ----------------------------------------------------------------------------
check_success(lw_grf500_set_stream(&grf500.device, LW_GRF500_STREAM_ID_NONE), "Failed to set stream: none\\n");

printf("Sample completed\\n");

return 0;

}
