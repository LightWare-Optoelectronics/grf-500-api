// ----------------------------------------------------------------------------
// LightWare Serial API
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
#define LW_PACKET_START_BYTE 0xAA
#define LW_PACKET_SEND_SIZE 160

#ifdef LW_LARGE_PACKETS
#define LW_PACKET_RECV_SIZE 1024
#else
#define LW_PACKET_RECV_SIZE 64
#endif

typedef enum {
    LW_PARSESTATE_START,
    LW_PARSESTATE_FLAGS1,
    LW_PARSESTATE_FLAGS2,
    LW_PARSESTATE_PAYLOAD,
    LW_PARSESTATE_DONE,
} lw_packet_parse_state;

typedef struct {
    uint8_t data[LW_PACKET_SEND_SIZE];
    uint32_t data_size;
    uint8_t command_id;
} lw_request;

// Set alignment to 1 byte.
typedef struct {
    uint8_t data[LW_PACKET_RECV_SIZE];
    uint32_t data_size;
    uint32_t payload_size;
    lw_packet_parse_state parse_state;
    uint8_t command_id;
} lw_response;

/*
 * Initialize a response.
 *
 * @param response The response to initialize.
 */
void lw_init_response(lw_response *response);

/*
 * Create a CRC for a data buffer.
 *
 * @param data The data buffer.
 * @param size The size of the data buffer.
 * @return The CRC.
 */
uint16_t lw_create_crc(uint8_t *data, uint16_t size);

/*
 * Create a binary protocol packet.
 *
 * @param packet_buffer The buffer to write the packet to.
 * @param command_id The command ID.
 * @param write Whether the packet is a write or read packet.
 * @param data The data to write to the packet.
 * @param data_size The size of the data.
 * @return The number of bytes written to packet_buffer.
 */
uint32_t lw_create_packet(uint8_t *packet_buffer, uint8_t command_id, uint8_t write, uint8_t *data, uint32_t data_size);

/*
 * Feed a response byte by byte until a full response packet is completed.
 *
 * @param response The response to feed.
 * @param data The data byte.
 * @return LW_RESULT_SUCCESS if the response is complete, or LW_RESULT_AGAIN if more data is needed.
 */
lw_result lw_feed_response(lw_response *response, uint8_t data);

/*
 * Extracts data from the packet buffer after the header.
 *
 * @param packet_buffer The full packet buffer.
 * @param data The data buffer to write the extracted data to.
 * @param size The size of the data to extract.
 * @param offset The offset to start reading from AFTER the packet header.
 */
void lw_parse_packet_data(uint8_t *packet_buffer, uint8_t *data, uint32_t size, uint32_t offset);

// ----------------------------------------------------------------------------
// Request generators.
//
// These functions construct a request for a specific command type.
// ----------------------------------------------------------------------------

/*
 * Create a read request. No additional data is sent with read requests.
 *
 * @param request The request to create.
 * @param command_id The command ID.
 */
void lw_create_request_read(lw_request *request, uint8_t command_id);

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
// This means that timeouts and retries are handled automatically.
//
// The 'callback device' is used to execute platform level callbacks such as
// sending data over a serial port, sleeping, and getting the current time.
// ----------------------------------------------------------------------------
#define LW_REQUEST_RETRIES 4
#define LW_RESPONSE_TIMEOUT_MS 1000

#define LW_ANY_COMMAND 255

typedef struct lw_callback_device_s lw_callback_device;

/*
 * Sleep callback. This callback is called when the API wants to sleep for a
 * specified number of milliseconds. This callback should only return once the
 * sleep time has elapsed, however it can return early if necessary and the
 * API will re-issue the callback again.
 *
 * @param device The callback device.
 * @param time_ms The time to sleep in milliseconds.
 */
typedef void (*lw_device_callback_sleep)(lw_callback_device *device, uint32_t time_ms);

/*
 * Get time callback. This callback is called when the API wants to get the
 * current time in milliseconds. This is NOT wall clock time, but most usually
 * the time since the host device was powered on. The API does not care what
 * the value represents as long as it monotoniclly increases in milliseconds
 * elapsed since the last call to this function.
 *
 * @param device The callback device.
 * @return The current time in milliseconds.
 */
typedef uint32_t (*lw_device_callback_get_time_ms)(lw_callback_device *device);

/*
 * Serial send callback. This callback is called when the API wants to send
 * data to the device. The callback should block until ALL the data has
 * been sent. Please take note of the return value requirements.
 *
 * @param device The callback device.
 * @param buffer The data buffer to send.
 * @param size The size of the data buffer.
 * @return Positive number: The number of bytes sent.
 *         0: If there was a critical error and the connection is lost.
 */
typedef uint32_t (*lw_device_callback_serial_send)(lw_callback_device *device, uint8_t *buffer, uint32_t size);

/*
 * Serial receive callback. This callback is called when the API wants to
 * receive data from the device. This function must receive UP TO the number
 * of bytes requested. If timeout_ms is 0 then the API assumes a non-blocking
 * call that will return immediately if no data is available. If timeout_ms
 * is non-zero then the function can block for the entire duration of
 * timeout_ms, but this is not required and the API will re-issue the callback
 * if necessary. Please take note of the return value requirements.
 *
 * @param device The callback device.
 * @param buffer The buffer to write the received data to.
 * @param size The number of bytes requested to read.
 * @param timeout_ms The timeout in milliseconds, or 0 for non-blocking.
 * @return Positive number: The number of bytes read.
 *         0: If timeout expired or no data is available.
 *         -1: If there was a critical error and the connection is lost.
 */
typedef int32_t (*lw_device_callback_serial_receive)(lw_callback_device *device, uint8_t *buffer, uint32_t size, uint32_t timeout_ms);

struct lw_callback_device_s {
    void *user_data;

    lw_device_callback_sleep sleep;
    lw_device_callback_get_time_ms get_time_ms;
    lw_device_callback_serial_send serial_send;
    lw_device_callback_serial_receive serial_receive;

    lw_request request;
    lw_response response;
};

/*
 * Create a managed callback device.
 *
 * @param user_data User data to pass to the platform callbacks.
 * @param sleep Sleep callback.
 * @param get_time_ms Get time callback.
 * @param serial_send Serial send callback.
 * @param serial_receive Serial receive callback.
 * @return The created callback device.
 */
lw_callback_device lw_create_callback_device(void *user_data,
                                             lw_device_callback_sleep sleep,
                                             lw_device_callback_get_time_ms get_time_ms,
                                             lw_device_callback_serial_send serial_send,
                                             lw_device_callback_serial_receive serial_receive);

/*
 * Wait for the next response packet with a specific command ID. This can be a
 * blocking or non-blocking call depending on the timeout_ms argument.
 *
 * @param device The callback device.
 * @param command_id The command ID to wait for, or LW_ANY_COMMAND.
 * @param timeout_ms The timeout in milliseconds, or 0 for non-blocking.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure, or
 *         LW_RESULT_AGAIN if non-blocking and no response has been completed.
 */
lw_result lw_wait_for_next_response(lw_callback_device *device, uint8_t command_id, uint32_t timeout_ms);

/*
 * Fully managed request sending and waiting for the response.
 *
 * @param device The callback device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_send_request_get_response(lw_callback_device *device);

#ifdef __cplusplus
}
#endif

#endif // LW_API_H
