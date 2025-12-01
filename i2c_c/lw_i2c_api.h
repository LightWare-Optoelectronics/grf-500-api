// ----------------------------------------------------------------------------
// LightWare I2C API
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
#ifndef LW_API_H
#define LW_API_H

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// Debug helpers.
//
// The API logs debug information if LW_DEBUG_LEVEL is set to 1 or higher.
//
// If LW_DEBUG_LEVEL is at least 1, then you must define the lw_debug_print
// function in your application:
//
// void lw_debug_print(const char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     vprintf(format, args);
//     va_end(args);
// }
//
// You can define LW_DEBUG_LEVEL as a compiler flag, eg: -DLW_DEBUG_LEVEL=1
// or just set it here.
// ----------------------------------------------------------------------------
#ifndef LW_DEBUG_LEVEL
#define LW_DEBUG_LEVEL 0
#endif

#if LW_DEBUG_LEVEL > 0
extern void lw_debug_print(const char *format, ...);
#define LW_DEBUG_LVL_1(...) lw_debug_print(__VA_ARGS__)
#else
#define LW_DEBUG_LVL_1(...)
#endif

#if LW_DEBUG_LEVEL > 1
void print_hex_debug(const char *prefix, uint8_t *data, uint32_t size);
#define LW_DEBUG_LVL_2(...) lw_debug_print(__VA_ARGS__)
#else
#define print_hex_debug(...) ;
#define LW_DEBUG_LVL_2(...)
#endif

#if LW_DEBUG_LEVEL > 2
#define LW_DEBUG_LVL_3(...) lw_debug_print(__VA_ARGS__)
#else
#define LW_DEBUG_LVL_3(...)
#endif

// ----------------------------------------------------------------------------
// Error handling.
//
// The macros here cut down on the verbosity of the API.
// ----------------------------------------------------------------------------
#define LW_CHECK_SUCCESS(expression)       \
    {                                      \
        lw_result result = expression;     \
        if (result != LW_RESULT_SUCCESS) { \
            return result;                 \
        }                                  \
    }

#define LW_CHECK_COMMAND_ID(req_resp, id)      \
    if (req_resp->command_id != id) {          \
        return LW_RESULT_INCORRECT_COMMAND_ID; \
    }

typedef enum {
    LW_RESULT_SUCCESS = 0,
    LW_RESULT_ERROR,
    LW_RESULT_AGAIN,
    LW_RESULT_TIMEOUT,
    LW_RESULT_EXCEEDED_RETRIES,
    LW_RESULT_INVALID_PARAMETER,
    LW_RESULT_INCORRECT_COMMAND_ID,
} lw_result;

typedef enum {
    LW_FALSE = 0,
    LW_TRUE = 1,
} lw_bool;

// ----------------------------------------------------------------------------
//  Parsing helpers.
// ----------------------------------------------------------------------------
typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} lw_firmware_version;

/*
 * Expand a 32-bit firmware version into a lw_firmware_version struct.
 *
 * @param version The 32-bit firmware version.
 * @return The expanded firmware version.
 */
lw_firmware_version lw_expand_firmware_version(uint32_t version);

// ----------------------------------------------------------------------------
// Packet handling.
//
// The functions here construct and parse binary packets in the simplest form.
// ----------------------------------------------------------------------------
#define LW_PACKET_SEND_SIZE 160

#ifdef LW_LARGE_PACKETS
#define LW_PACKET_RECV_SIZE 1024
#else
#define LW_PACKET_RECV_SIZE 64
#endif

typedef struct {
    uint8_t data[LW_PACKET_SEND_SIZE];
    uint32_t data_size;
    uint8_t command_id;
    uint8_t write;
} lw_request;

typedef struct {
    uint8_t data[LW_PACKET_RECV_SIZE];
    uint32_t data_size;
    uint8_t command_id;
} lw_response;

/*
 * Initialize a request.
 *
 * @param request The request to initialize.
 */
void lw_init_request(lw_request *request);

/*
 * Initialize a response.
 *
 * @param response The response to initialize.
 */
void lw_init_response(lw_response *response);

/*
 * Count the number of set bits in a 32-bit integer.
 *
 * @param value The value to count bits in.
 * @return The number of set bits.
 */
inline uint8_t lw_count_bits(uint32_t value) {
    int count = 0;
    while (value > 0) {
        value &= (value - 1);
        count++;
    }
    return count;
}

// ----------------------------------------------------------------------------
// Request generators.
//
// These functions construct a request for a specific command type.
// ----------------------------------------------------------------------------
/*
 * Create a read request for an int8_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_int8(lw_request *request, uint8_t command_id);

/*
 * Create a read request for an int16_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_int16(lw_request *request, uint8_t command_id);

/*
 * Create a read request for an int32_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_int32(lw_request *request, uint8_t command_id);

/*
 * Create a read request for a uint8_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_uint8(lw_request *request, uint8_t command_id);

/*
 * Create a read request for a uint16_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_uint16(lw_request *request, uint8_t command_id);

/*
 * Create a read request for a uint32_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_uint32(lw_request *request, uint8_t command_id);

/*
 * Create a read request for a 16-byte data buffer.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read_data16(lw_request *request, uint8_t command_id);

/*
 * Create a read request for an arbitrary amount of data.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param data_size The size of the data to read.
 */
void lw_create_request_read_data(lw_request *request, uint8_t command_id, uint32_t data_size);

/*
 * Create a write request with an int8_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The int8_t value to include.
 */
void lw_create_request_write_int8(lw_request *request, uint8_t command_id, int8_t value);

/*
 * Create a write request with an int16_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The int16_t value to include.
 */
void lw_create_request_write_int16(lw_request *request, uint8_t command_id, int16_t value);

/*
 * Create a write request with an int32_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The int32_t value to include.
 */
void lw_create_request_write_int32(lw_request *request, uint8_t command_id, int32_t value);

/*
 * Create a write request with a uint8_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The uint8_t value to include.
 */
void lw_create_request_write_uint8(lw_request *request, uint8_t command_id, uint8_t value);

/*
 * Create a write request with a uint16_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The uint16_t value to include.
 */
void lw_create_request_write_uint16(lw_request *request, uint8_t command_id, uint16_t value);

/*
 * Create a write request with a uint32_t value.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param value The uint32_t value to include.
 */
void lw_create_request_write_uint32(lw_request *request, uint8_t command_id, uint32_t value);

/*
 * Create a write request with a 16-byte string.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param string The string to write, must be a 16-byte buffer.
 */
void lw_create_request_write_string(lw_request *request, uint8_t command_id, char *string);

/*
 * Create a write request with an arbitrary amount of data.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 * @param data The data buffer to include.
 * @param data_size The size of the data buffer.
 */
void lw_create_request_write_data(lw_request *request, uint8_t command_id, uint8_t *data, uint32_t data_size);

// ----------------------------------------------------------------------------
// Response parsers.
//
// These functions parse a response for a specific command type.
// ----------------------------------------------------------------------------

/*
 * Get an int8_t value from a response.
 *
 * @param response The response to parse.
 * @param data The int8_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_int8(lw_response *response, int8_t *data, uint32_t offset);

/*
 * Get an int16_t value from a response.
 *
 * @param response The response to parse.
 * @param data The int16_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_int16(lw_response *response, int16_t *data, uint32_t offset);

/*
 * Get an int32_t value from a response.
 *
 * @param response The response to parse.
 * @param data The int32_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_int32(lw_response *response, int32_t *data, uint32_t offset);

/*
 * Get a uint8_t value from a response.
 *
 * @param response The response to parse.
 * @param data The uint8_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_uint8(lw_response *response, uint8_t *data, uint32_t offset);

/*
 * Get a uint16_t value from a response.
 *
 * @param response The response to parse.
 * @param data The uint16_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_uint16(lw_response *response, uint16_t *data, uint32_t offset);

/*
 * Get a uint32_t value from a response.
 *
 * @param response The response to parse.
 * @param data The uint32_t value to write to.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_uint32(lw_response *response, uint32_t *data, uint32_t offset);

/*
 * Get a 16-byte string from a response.
 *
 * @param response The response to parse.
 * @param data The string to write to, must be 16-bytes.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_string(lw_response *response, char *data, uint32_t offset);

/*
 * Get an arbitrary amount of data from a response.
 *
 * @param response The response to parse.
 * @param data The data buffer to write to.
 * @param size The size of the data buffer.
 * @param offset The offset to start reading from.
 */
void lw_parse_response_data(lw_response *response, uint8_t *data, uint32_t size, uint32_t offset);

// ----------------------------------------------------------------------------
// Managed request/response commands.
//
// The API offers a fully managed way to send requests and receive responses.
// This means that read and write calls are handled automatically.
//
// The 'callback device' is used to execute platform level callbacks which
// issue read and write commands over I2C.
// ----------------------------------------------------------------------------
typedef struct lw_callback_device_s lw_callback_device;

/*
 * I2C read callback. This callback is called when the API wants to send a
 * read request to the device.
 *
 * @param device The callback device.
 * @param reg The register to read from.
 * @param buffer The data buffer to be read into.
 * @param size The number of bytes to read.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
typedef lw_result (*lw_device_callback_i2c_read)(lw_callback_device *device, uint8_t reg,  uint8_t *buffer, uint32_t size);

/*
 * I2C write callback. This callback is called when the API wants to send a
 * write request to the device.
 *
 * @param device The callback device.
 * @param reg The register to write to.
 * @param buffer The data buffer to write.
 * @param size The number of bytes to write.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
typedef lw_result (*lw_device_callback_i2c_write)(lw_callback_device *device, uint8_t reg, uint8_t *buffer, uint32_t size);

struct lw_callback_device_s {
    void *user_data;

    lw_device_callback_i2c_read i2c_read;
    lw_device_callback_i2c_write i2c_write;

    lw_request request;
    lw_response response;
};

/*
 * Create a managed callback device.
 *
 * @param user_data User data to pass to the platform callbacks.
 * @param i2c_read I2C read callback.
 * @param i2c_write I2C write callback.
 * @return The created callback device.
 */
lw_callback_device lw_create_callback_device(void *user_data,
                                             lw_device_callback_i2c_read i2c_read,
                                             lw_device_callback_i2c_write i2c_write);

/*
 * Fully managed request sending. This function will use the callbacks
 * defined in the callback device to send the request and receive data.
 *
 * @param device The callback device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_send_request_get_response(lw_callback_device *device);

#ifdef __cplusplus
}
#endif

#endif // LW_API_H
