// ----------------------------------------------------------------------------
// LightWare I2C API
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
#include "lw_i2c_api.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Error handling.
// ----------------------------------------------------------------------------
#if LW_DEBUG_LEVEL > 1
void print_hex_debug(const char *prefix, uint8_t *data, uint32_t size) {
    LW_DEBUG_LVL_2("%s", prefix);

    for (uint32_t i = 0; i < size; ++i) {
        LW_DEBUG_LVL_2("0x%02X ", data[i]);
    }

    LW_DEBUG_LVL_2("\n");
}
#endif

// ----------------------------------------------------------------------------
//  Parsing helpers.
// ----------------------------------------------------------------------------
lw_firmware_version lw_expand_firmware_version(uint32_t version) {
    lw_firmware_version result;
    result.major = (version >> 16) & 0xFF;
    result.minor = (version >> 8) & 0xFF;
    result.patch = (version >> 0) & 0xFF;

    return result;
}

// ----------------------------------------------------------------------------
// Packet handling.
// ----------------------------------------------------------------------------
void lw_init_request(lw_request *request) {
    request->data_size = 0;
    request->command_id = 0;
    request->write = 0;
}

void lw_init_response(lw_response *response) {
    response->data_size = 0;
    response->command_id = UINT8_MAX;
}

// ----------------------------------------------------------------------------
// Request generators.
// ----------------------------------------------------------------------------
inline void lw_create_request_read(lw_request *request, uint8_t command_id, uint32_t data_size) {
    request->data_size = data_size;
    request->command_id = command_id;
    request->write = 0;
}

void lw_create_request_read_int8(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 1);
}

void lw_create_request_read_int16(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 2);
}

void lw_create_request_read_int32(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 4);
}

void lw_create_request_read_uint8(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 1);
}

void lw_create_request_read_uint16(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 2);
}

void lw_create_request_read_uint32(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 4);
}

void lw_create_request_read_data16(lw_request *request, uint8_t command_id) {
    lw_create_request_read(request, command_id, 16);
}

void lw_create_request_read_data(lw_request *request, uint8_t command_id, uint32_t data_size) {
    lw_create_request_read(request, command_id, data_size);
}

inline void lw_create_write_request(lw_request *request, uint8_t command_id, uint8_t *data, uint32_t data_size) {
    request->data_size = data_size;
    request->command_id = command_id;
    request->write = 1;
    memcpy(request->data, data, data_size);
}

void lw_create_request_write_int8(lw_request *request, uint8_t command_id, int8_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 1);
}

void lw_create_request_write_int16(lw_request *request, uint8_t command_id, int16_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 2);
}

void lw_create_request_write_int32(lw_request *request, uint8_t command_id, int32_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 4);
}

void lw_create_request_write_uint8(lw_request *request, uint8_t command_id, uint8_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 1);
}

void lw_create_request_write_uint16(lw_request *request, uint8_t command_id, uint16_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 2);
}

void lw_create_request_write_uint32(lw_request *request, uint8_t command_id, uint32_t value) {
    lw_create_write_request(request, command_id, (uint8_t *)&value, 4);
}

void lw_create_request_write_string(lw_request *request, uint8_t command_id, char *string) {
    lw_create_write_request(request, command_id, (uint8_t *)string, 16);
}

void lw_create_request_write_data(lw_request *request, uint8_t command_id, uint8_t *data, uint32_t data_size) {
    lw_create_write_request(request, command_id, data, data_size);
}

// ----------------------------------------------------------------------------
// Response parsers.
// ----------------------------------------------------------------------------
inline void lw_parse_packet_data(uint8_t *packet_buffer, uint8_t *data, uint32_t size, uint32_t offset) {
    memcpy(data, packet_buffer + offset, size);
}

void lw_parse_response_int8(lw_response *response, int8_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 1, offset);
}

void lw_parse_response_int16(lw_response *response, int16_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 2, offset);
}

void lw_parse_response_int32(lw_response *response, int32_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 4, offset);
}

void lw_parse_response_uint8(lw_response *response, uint8_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, data, 1, offset);
}

void lw_parse_response_uint16(lw_response *response, uint16_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 2, offset);
}

void lw_parse_response_uint32(lw_response *response, uint32_t *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 4, offset);
}

void lw_parse_response_string(lw_response *response, char *data, uint32_t offset) {
    lw_parse_packet_data(response->data, (uint8_t *)data, 16, offset);
}

void lw_parse_response_data(lw_response *response, uint8_t *data, uint32_t size, uint32_t offset) {
    lw_parse_packet_data(response->data, data, size, offset);
}

// ----------------------------------------------------------------------------
// Managed request/response commands.
// ----------------------------------------------------------------------------
lw_callback_device lw_create_callback_device(void *user_data,
                                             lw_device_callback_i2c_read i2c_read,
                                             lw_device_callback_i2c_write i2c_write) {

    lw_callback_device device = {0};
    device.user_data = user_data;
    device.i2c_read = i2c_read;
    device.i2c_write = i2c_write;

    lw_init_request(&device.request);
    lw_init_response(&device.response);

    return device;
}

lw_result lw_send_request_get_response(lw_callback_device *device) {
    LW_DEBUG_LVL_3("Sending I2C request\n");

    if (device->request.write) {
        LW_DEBUG_LVL_3("I2C Write: CMD=0x%02X, SIZE=%d\n", device->request.command_id, device->request.data_size);
        print_hex_debug("I2C Write Data: ", device->request.data, device->request.data_size);

        lw_result result = device->i2c_write(device, device->request.command_id, device->request.data, device->request.data_size);
        
        if (result != LW_RESULT_SUCCESS) {
            return result;
        }
    } else {
        LW_DEBUG_LVL_3("I2C Read: CMD=0x%02X, SIZE=%d\n", device->request.command_id, device->request.data_size);

        device->response.data_size = device->request.data_size;
        device->response.command_id = device->request.command_id;
        lw_result result = device->i2c_read(device, device->request.command_id, device->response.data, device->response.data_size);
        
        if (result != LW_RESULT_SUCCESS) {
            return result;
        }

        print_hex_debug("I2C Read Data: ", device->response.data, device->response.data_size);
    }

    return LW_RESULT_SUCCESS;
}
