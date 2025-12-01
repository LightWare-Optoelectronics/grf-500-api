// ----------------------------------------------------------------------------
// LightWare Serial API
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
#include "lw_serial_api.h"
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
uint16_t lw_create_crc(uint8_t *data, uint16_t size) {
    uint16_t crc = 0;

    for (uint32_t i = 0; i < size; ++i) {
        uint16_t code = crc >> 8;
        code ^= data[i];
        code ^= code >> 4;
        crc = crc << 8;
        crc ^= code;
        code = code << 5;
        crc ^= code;
        code = code << 7;
        crc ^= code;
    }

    return crc;
}

void lw_init_response(lw_response *response) {
    response->data_size = 0;
    response->payload_size = 0;
    response->parse_state = LW_PARSESTATE_START;
    response->command_id = UINT8_MAX;
}

lw_result lw_feed_response(lw_response *response, uint8_t data) {
    LW_DEBUG_LVL_3("Feed packet: 0x%02X\n", data);
    
    if (response->parse_state == LW_PARSESTATE_DONE) {
        lw_init_response(response);
    }

    switch (response->parse_state) {
        case LW_PARSESTATE_START: {
            if (data == LW_PACKET_START_BYTE) {
                response->parse_state = LW_PARSESTATE_FLAGS1;
                response->data[0] = LW_PACKET_START_BYTE;
            }

            break;
        }

        case LW_PARSESTATE_FLAGS1: {
            response->parse_state = LW_PARSESTATE_FLAGS2;
            response->data[1] = data;
            break;
        }

        case LW_PARSESTATE_FLAGS2: {
            response->parse_state = LW_PARSESTATE_PAYLOAD;
            response->data[2] = data;
            response->data_size = 3;
            response->payload_size = (uint32_t)(response->data[1] | (response->data[2] << 8)) >> 6;

            if (response->payload_size > (LW_PACKET_RECV_SIZE - 5) || response->payload_size < 1) {
                response->parse_state = LW_PARSESTATE_START;
                LW_DEBUG_LVL_2("Invalid payload size %d\n", response->payload_size);
            }

            break;
        }

        case LW_PARSESTATE_PAYLOAD: {
            response->data[response->data_size++] = data;

            if (response->data_size == response->payload_size + 5) {
                uint16_t crc = response->data[response->data_size - 2] | (response->data[response->data_size - 1] << 8);
                uint16_t verify_crc = lw_create_crc(response->data, (uint16_t)(response->data_size - 2));

                if (crc == verify_crc) {
                    response->parse_state = LW_PARSESTATE_DONE;
                    response->command_id = response->data[3];
                    print_hex_debug("Recv packet: ", response->data, response->data_size);
                    LW_DEBUG_LVL_2("Got packet %d\n", response->command_id);
                    return LW_RESULT_SUCCESS;
                } else {
                    response->parse_state = LW_PARSESTATE_START;
                    LW_DEBUG_LVL_2("Invalid CRC\n");
                }
            }

            break;
        }

        case LW_PARSESTATE_DONE: {
            return LW_RESULT_ERROR;
        }
    }

    return LW_RESULT_AGAIN;
}

uint32_t lw_create_packet(uint8_t *packet_buffer, uint8_t command_id, uint8_t write, uint8_t *data, uint32_t data_size) {
    uint32_t payload_length = 1 + data_size;
    uint16_t flags = (uint16_t)((payload_length << 6) | (write & 0x1));

    packet_buffer[0] = LW_PACKET_START_BYTE;    // Start byte.
    packet_buffer[1] = ((uint8_t *)&flags)[0];  // Flags low.
    packet_buffer[2] = ((uint8_t *)&flags)[1];  // Flags high.
    packet_buffer[3] = command_id;              // Payload: Command ID.
    memcpy(packet_buffer + 4, data, data_size); // Payload: Data.
    uint16_t crc = lw_create_crc(packet_buffer, (uint16_t)(4 + data_size));
    packet_buffer[4 + data_size] = ((uint8_t *)&crc)[0]; // Checksum low.
    packet_buffer[5 + data_size] = ((uint8_t *)&crc)[1]; // Checksum high.

    return 6 + data_size;
}

void lw_parse_packet_data(uint8_t *packet_buffer, uint8_t *data, uint32_t size, uint32_t offset) {
    memcpy(data, packet_buffer + 4 + offset, size);
}

// ----------------------------------------------------------------------------
// Request generators.
// ----------------------------------------------------------------------------
static void lw_init_request(lw_request *request, uint8_t command_id, uint32_t size) {
    request->data_size = size;
    request->command_id = command_id;
}

void lw_create_request_read(lw_request *request, uint8_t command_id) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 0, NULL, 0));
}

void lw_create_request_write_int8(lw_request *request, uint8_t command_id, int8_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 1));
}

void lw_create_request_write_int16(lw_request *request, uint8_t command_id, int16_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 2));
}

void lw_create_request_write_int32(lw_request *request, uint8_t command_id, int32_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 4));
}

void lw_create_request_write_uint8(lw_request *request, uint8_t command_id, uint8_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 1));
}

void lw_create_request_write_uint16(lw_request *request, uint8_t command_id, uint16_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 2));
}

void lw_create_request_write_uint32(lw_request *request, uint8_t command_id, uint32_t value) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)&value, 4));
}

void lw_create_request_write_string(lw_request *request, uint8_t command_id, char *string) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, (uint8_t *)string, 16));
}

void lw_create_request_write_data(lw_request *request, uint8_t command_id, uint8_t *data, uint32_t data_size) {
    lw_init_request(request, command_id, lw_create_packet(request->data, command_id, 1, data, data_size));
}

// ----------------------------------------------------------------------------
// Response parsers.
// ----------------------------------------------------------------------------
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
                                             lw_device_callback_sleep sleep,
                                             lw_device_callback_get_time_ms get_time_ms,
                                             lw_device_callback_serial_send serial_send,
                                             lw_device_callback_serial_receive serial_receive) {

    lw_callback_device device = {0};
    device.user_data = user_data;
    device.sleep = sleep;
    device.get_time_ms = get_time_ms;
    device.serial_send = serial_send;
    device.serial_receive = serial_receive;

    lw_init_request(&device.request, 0, 0);
    lw_init_response(&device.response);

    return device;
}

lw_result lw_wait_for_next_response(lw_callback_device *device, uint8_t command_id, uint32_t timeout_ms) {
    uint32_t timeout_time = 0;

    if (timeout_ms != 0) {
        timeout_time = device->get_time_ms(device) + timeout_ms;
    }

    while (1) {
        uint32_t current_time = 0;
        uint32_t time_left_ms = 0;

        if (timeout_ms != 0) {
            current_time = device->get_time_ms(device);

            if (current_time < timeout_time) {
                time_left_ms = timeout_time - current_time;
            }
        }

        uint8_t byte = 0;
        int32_t bytes_read = device->serial_receive(device, &byte, 1, time_left_ms);

        if (bytes_read == -1) {
            return LW_RESULT_ERROR;
        } else if (bytes_read > 0) {
            if (lw_feed_response(&device->response, byte) == LW_RESULT_SUCCESS) {
                if (command_id == LW_ANY_COMMAND || device->response.command_id == command_id) {
                    return LW_RESULT_SUCCESS;
                }
            }
        } else if (timeout_ms == 0) {
            return LW_RESULT_AGAIN;
        } else if (time_left_ms == 0) {
            return LW_RESULT_TIMEOUT;
        }
    }
}

lw_result lw_send_request_get_response(lw_callback_device *device) {
    LW_DEBUG_LVL_3("Running request\n");

    int32_t attempts = LW_REQUEST_RETRIES;

    while (attempts--) {
        print_hex_debug("Send packet: ", device->request.data, device->request.data_size);
        if (device->serial_send(device, device->request.data, device->request.data_size) == 0) {
            return LW_RESULT_ERROR;
        }

        lw_result result = lw_wait_for_next_response(device, device->request.command_id, LW_RESPONSE_TIMEOUT_MS);

        if (result == LW_RESULT_SUCCESS) {
            return LW_RESULT_SUCCESS;
        }

        if (result == LW_RESULT_ERROR) {
            return LW_RESULT_ERROR;
        }

        LW_DEBUG_LVL_2("Timeout waiting for packet: %d attemps remaining\n", attempts);
    }

    return LW_RESULT_EXCEEDED_RETRIES;
}
