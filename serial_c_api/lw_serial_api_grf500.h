// ----------------------------------------------------------------------------
// LightWare Serial API GRF-500
// Version: 1.1.0
// Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
// https://www.lightwarelidar.com
// ----------------------------------------------------------------------------
//
// License: MIT
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------
#ifndef LW_API_GRF500_H
#define LW_API_GRF500_H

#include "lw_serial_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// Command IDs.
// ----------------------------------------------------------------------------
#define LW_GRF500_COMMAND_PRODUCT_NAME 0
#define LW_GRF500_COMMAND_HARDWARE_VERSION 1
#define LW_GRF500_COMMAND_FIRMWARE_VERSION 2
#define LW_GRF500_COMMAND_SERIAL_NUMBER 3
#define LW_GRF500_COMMAND_USER_DATA 9
#define LW_GRF500_COMMAND_TOKEN 10
#define LW_GRF500_COMMAND_SAVE_PARAMETERS 12
#define LW_GRF500_COMMAND_RESET 14
#define LW_GRF500_COMMAND_DISTANCE_CONFIG 27
#define LW_GRF500_COMMAND_STREAM 30
#define LW_GRF500_COMMAND_DISTANCE_DATA 44
#define LW_GRF500_COMMAND_MULTI_DATA 45
#define LW_GRF500_COMMAND_LASER_FIRING 50
#define LW_GRF500_COMMAND_TEMPERATURE 55
#define LW_GRF500_COMMAND_AUTO_EXPOSURE 70
#define LW_GRF500_COMMAND_UPDATE_RATE 74
#define LW_GRF500_COMMAND_ALARM_STATUS 76
#define LW_GRF500_COMMAND_ALARM_RETURN_MODE 77
#define LW_GRF500_COMMAND_LOST_SIGNAL_COUNTER 78
#define LW_GRF500_COMMAND_ALARM_A_DISTANCE 79
#define LW_GRF500_COMMAND_ALARM_B_DISTANCE 80
#define LW_GRF500_COMMAND_ALARM_HYSTERESIS 81
#define LW_GRF500_COMMAND_GPIO_MODE 83
#define LW_GRF500_COMMAND_GPIO_ALARM_CONFIRM_COUNT 84
#define LW_GRF500_COMMAND_MEDIAN_FILTER_ENABLE 86
#define LW_GRF500_COMMAND_MEDIAN_FILTER_SIZE 87
#define LW_GRF500_COMMAND_SMOOTH_FILTER_ENABLE 88
#define LW_GRF500_COMMAND_SMOOTH_FILTER_FACTOR 89
#define LW_GRF500_COMMAND_BAUD_RATE 91
#define LW_GRF500_COMMAND_I2C_ADDRESS 92
#define LW_GRF500_COMMAND_ROLLING_AVERAGE_ENABLE 93
#define LW_GRF500_COMMAND_ROLLING_AVERAGE_SIZE 94
#define LW_GRF500_COMMAND_SLEEP 98
#define LW_GRF500_COMMAND_LED_STATE 110
#define LW_GRF500_COMMAND_ZERO_OFFSET 114


// ----------------------------------------------------------------------------
// Per-command types.
// ----------------------------------------------------------------------------
typedef enum {
    LW_GRF500_STREAM_ID_NONE = 0,
    LW_GRF500_STREAM_ID_DISTANCE_DATA = 5,
    LW_GRF500_STREAM_ID_MULTI_DATA = 6,
} lw_grf500_stream_id;

typedef struct {
    char product_name[16];
    uint32_t hardware_version;
    lw_firmware_version firmware_version;
    char serial_number[16];
} lw_grf500_product_info;

typedef uint32_t lw_grf500_distance_config;
#define LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_RAW (1 << 0)
#define LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_FILTERED (1 << 1)
#define LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_STRENGTH (1 << 2)
#define LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_RAW (1 << 3)
#define LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_FILTERED (1 << 4)
#define LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_STRENGTH (1 << 5)
#define LW_GRF500_DISTANCE_CONFIG_TEMPERATURE (1 << 6)
#define LW_GRF500_DISTANCE_CONFIG_ALARM_STATUS (1 << 7)
#define LW_GRF500_DISTANCE_CONFIG_ALL (0xFF)

typedef enum {
    LW_GRF500_RETURN_MODE_FIRST = 0,
    LW_GRF500_RETURN_MODE_LAST = 1,
} lw_grf500_return_mode;

typedef enum {
    LW_GRF500_BAUD_RATE_9600 = 0,
    LW_GRF500_BAUD_RATE_19200 = 1,
    LW_GRF500_BAUD_RATE_38400 = 2,
    LW_GRF500_BAUD_RATE_57600 = 3,
    LW_GRF500_BAUD_RATE_115200 = 4,
    LW_GRF500_BAUD_RATE_230400 = 5,
    LW_GRF500_BAUD_RATE_460800 = 6,
    LW_GRF500_BAUD_RATE_921600 = 7,
} lw_grf500_baud_rate;

typedef struct {
	uint8_t alarm_a;
	uint8_t alarm_b;
} lw_grf500_alarm_status;


typedef enum {
    LW_GRF500_GPIO_MODE_NO_OUTPUT = 0, // Output of 0 V.
    LW_GRF500_GPIO_MODE_ALARM_A = 1, // Output of 3.3 V when alarm A is active.
    LW_GRF500_GPIO_MODE_ALARM_B = 2, // Output of 3.3 V when alarm B is active.
} lw_grf500_gpio_mode;

typedef struct {
    int32_t first_return_raw_cm;
    int32_t first_return_filtered_cm;
    int32_t first_return_strength;
    int32_t last_return_raw_cm;
    int32_t last_return_filtered_cm;
    int32_t last_return_strength;
    int32_t temperature;
    int32_t alarm_status;
} lw_grf500_distance_data_cm;

typedef struct {
	int32_t distance_mm;
	int32_t strength;
} lw_grf500_multi_data_signal;

typedef struct {
	lw_grf500_multi_data_signal signals[5];
	int32_t temperature;
} lw_grf500_multi_data;




// ----------------------------------------------------------------------------
// Fully managed request/response commands.
//
// These functions handle sending and receiving of requests and responses.
// Timeouts and retries are handled automatically.
//
// The 'callback device' is used to execute platform level callbacks such as
// sending data over a serial port, sleeping, and getting the current time.
// ----------------------------------------------------------------------------

/*
 * Get the product name.
 *
 * @param device Connected device.
 * @param product_name A 16-byte buffer where product name is written.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_product_name(lw_callback_device *device, char *product_name);

/*
 * Get the hardware version.
 *
 * @param device Connected device.
 * @param hardware_version The hardware version of the device is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_hardware_version(lw_callback_device *device, uint32_t *hardware_version);

/*
 * Get the firmware version.
 *
 * @param device Connected device.
 * @param firmware_version The firmware version of the device is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_firmware_version(lw_callback_device *device, lw_firmware_version *firmware_version);

/*
 * Get the serial number.
 *
 * @param device Connected device.
 * @param serial_number A 16-byte buffer where the serial number is written.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_serial_number(lw_callback_device *device, char *serial_number);

/*
 * Get the user data.
 *
 * @param device Connected device.
 * @param data A 16-byte buffer where user data is written.
 * @param data_size Size of the data buffer.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_user_data(lw_callback_device *device, uint8_t *data, uint32_t data_size);

/*
 * Write 16-bytes of user data to the device.
 *
 * @param device Connected device.
 * @param data A 16-byte buffer of user data.
 * @param data_size Size of the data buffer.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_user_data(lw_callback_device *device, uint8_t *data, uint32_t data_size);

/*
 * Get the next usable safety token for saving parameters or resetting the device.
 *
 * @param device Connected device.
 * @param token The next usable safety token is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_token(lw_callback_device *device, uint16_t *token);

/*
 * Save the current persistable device parameters.
 *
 * @param device Connected device.
 * @param token The latest unused safety token
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_save_parameters(lw_callback_device *device, uint16_t token);

/*
 * Restart the device as if it had been power cycled.
 *
 * @param device Connected device.
 * @param token The latest unused safety token.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_reset(lw_callback_device *device, uint16_t token);

/*
 * Get the distance configuration flags. Used to determine which data will be available for the distance data command. See DistanceConfig for flag definitions.
 *
 * @param device Connected device.
 * @param config The distance configuration flags are written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_distance_config(lw_callback_device *device, lw_grf500_distance_config *config);

/*
 * Set the distance configuration flags. Used to determine which data will be available for the distance data command. See DistanceConfig for flag definitions.
 *
 * @param device Connected device.
 * @param config The configuration configuration flags.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_distance_config(lw_callback_device *device, lw_grf500_distance_config config);

/*
 * Get the currently set stream.
 *
 * @param device Connected device.
 * @param stream The current stream is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_stream(lw_callback_device *device, lw_grf500_stream_id *stream);

/*
 * Set the stream.
 *
 * @param device Connected device.
 * @param stream The stream to set.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_stream(lw_callback_device *device, lw_grf500_stream_id stream);

/*
 * Get the distance data. Only values as defined by the distance configuration will be available.
 *
 * @param device Connected device.
 * @param config The distance configuration flags.
 * @param data The distance data is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_distance_data(lw_callback_device *device, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data);

/*
 * Get 5 signals from the device. Each signal contains a distance and strength. If more than one target is identified within its field of view, the device will report up to five discrete target distances.
 *
 * @param device Connected device.
 * @param data The multi signal data is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_multi_data(lw_callback_device *device, lw_grf500_multi_data *data);

/*
 * Get the laser firing state.
 *
 * @param device Connected device.
 * @param enable The laser firing state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_laser_firing(lw_callback_device *device, lw_bool *enable);

/*
 * Set the laser firing state.
 *
 * @param device Connected device.
 * @param enable The laser firing state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_laser_firing(lw_callback_device *device, lw_bool enable);

/*
 * Get the temperature.
 *
 * @param device Connected device.
 * @param temperature The temperature in 100th of a degree Celsius is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_temperature(lw_callback_device *device, int32_t *temperature);

/*
 * Get the auto exposure state.
 *
 * @param device Connected device.
 * @param enable The auto exposure state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_auto_exposure(lw_callback_device *device, lw_bool *enable);

/*
 * Set the auto exposure state.
 *
 * @param device Connected device.
 * @param enable The auto exposure state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_auto_exposure(lw_callback_device *device, lw_bool enable);

/*
 * Get the update rate.
 *
 * @param device Connected device.
 * @param rate The update rate in Hz is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_update_rate(lw_callback_device *device, float *rate);

/*
 * Set the update rate.
 *
 * @param device Connected device.
 * @param rate The update rate in Hz.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_update_rate(lw_callback_device *device, float rate);

/*
 * Get the alarm status. A value of 0 means the alarm is not active. A value of 1 means the alarm is active.
 *
 * @param device Connected device.
 * @param status The alarm status of Alarm A and Alarm B is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_alarm_status(lw_callback_device *device, lw_grf500_alarm_status *status);

/*
 * Get the return mode used for alarms.
 *
 * @param device Connected device.
 * @param mode The alarm return mode is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_alarm_return_mode(lw_callback_device *device, lw_grf500_return_mode *mode);

/*
 * Set the return mode used for alarms.
 *
 * @param device Connected device.
 * @param mode The alarm return mode.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_alarm_return_mode(lw_callback_device *device, lw_grf500_return_mode mode);

/*
 * Get the number of lost signal returns before a lost signal indication is output on the distance value.
Lost signal value will be -1000 mm.
 *
 * @param device Connected device.
 * @param counter The lost signal counter is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_lost_signal_counter(lw_callback_device *device, uint32_t *counter);

/*
 * Set the number of lost signal returns before a lost signal indication is output on the distance value. Lost signal value will be -1000 mm.
 *
 * @param device Connected device.
 * @param counter The lost signal counter.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_lost_signal_counter(lw_callback_device *device, uint32_t counter);

/*
 * Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.
 *
 * @param device Connected device.
 * @param distance_cm The alarm A distance in cm is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_alarm_a_distance(lw_callback_device *device, uint32_t *distance_cm);

/*
 * Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.
 *
 * @param device Connected device.
 * @param distance_cm The alarm A distance in cm.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_alarm_a_distance(lw_callback_device *device, uint32_t distance_cm);

/*
 * Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.
 *
 * @param device Connected device.
 * @param distance_cm The alarm B distance in cm is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_alarm_b_distance(lw_callback_device *device, uint32_t *distance_cm);

/*
 * Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.
 *
 * @param device Connected device.
 * @param distance_cm The alarm B distance in cm.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_alarm_b_distance(lw_callback_device *device, uint32_t distance_cm);

/*
 * The hysteresis setting is used to prevent the alarm from toggling on and off when the distance is near the alarm set point.
 *
 * @param device Connected device.
 * @param hysteresis_cm The alarm hysterisis in cm is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_alarm_hysteresis(lw_callback_device *device, uint32_t *hysteresis_cm);

/*
 * The hysteresis setting is used to prevent the alarm from toggling on and off when the distance is near the alarm set point.
 *
 * @param device Connected device.
 * @param hysteresis_cm The alarm hysteresis in cm.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_alarm_hysteresis(lw_callback_device *device, uint32_t hysteresis_cm);

/*
 * Get the output mode of the alarm pin on the external connector. See GpioMode for mode definitions.
 *
 * @param device Connected device.
 * @param mode The alarm pin output mode is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_gpio_mode(lw_callback_device *device, lw_grf500_gpio_mode *mode);

/*
 * Set the output mode of the alarm pin on the external connector. See GpioMode for mode definitions.
 *
 * @param device Connected device.
 * @param mode The alarm pin output mode.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_gpio_mode(lw_callback_device *device, lw_grf500_gpio_mode mode);

/*
 * Get the number of confirmed alarm signals before the alarm pin on the external connector is activated. When the alarm resets the pin is immediately deactivated.
 *
 * @param device Connected device.
 * @param count The number of confirmations is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_gpio_alarm_confirm_count(lw_callback_device *device, uint32_t *count);

/*
 * Set the number of confirmed alarm signals before the alarm pin on the external connector is activated. When the alarm resets the pin is immediately deactivated.
 *
 * @param device Connected device.
 * @param count Number of confirmations.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_gpio_alarm_confirm_count(lw_callback_device *device, uint32_t count);

/*
 * Get the enabled state of the median filter.
 *
 * @param device Connected device.
 * @param enable The median filter state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_median_filter_enable(lw_callback_device *device, lw_bool *enable);

/*
 * Set the enabled state of the median filter.
 *
 * @param device Connected device.
 * @param enable The median filter state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_median_filter_enable(lw_callback_device *device, lw_bool enable);

/*
 * Get the size of the median filter.
 *
 * @param device Connected device.
 * @param size The median filter size is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_median_filter_size(lw_callback_device *device, uint32_t *size);

/*
 * Set the size of the median filter.
 *
 * @param device Connected device.
 * @param size The median filter size.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_median_filter_size(lw_callback_device *device, uint32_t size);

/*
 * Get the enabled state of the smooth filter.
 *
 * @param device Connected device.
 * @param enable The smooth filter state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_smooth_filter_enable(lw_callback_device *device, lw_bool *enable);

/*
 * Set the enabled state of the smooth filter.
 *
 * @param device Connected device.
 * @param enable The smooth filter state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_smooth_filter_enable(lw_callback_device *device, lw_bool enable);

/*
 * Get the factor of the smooth filter.
 *
 * @param device Connected device.
 * @param factor The smooth filter factor is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_smooth_filter_factor(lw_callback_device *device, uint32_t *factor);

/*
 * Set the factor of the smooth filter.
 *
 * @param device Connected device.
 * @param factor The smooth filter factor.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_smooth_filter_factor(lw_callback_device *device, uint32_t factor);

/*
 * Get the baud rate used by the serial interface.
 *
 * @param device Connected device.
 * @param baud_rate The baud rate is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_baud_rate(lw_callback_device *device, lw_grf500_baud_rate *baud_rate);

/*
 * Set the baud rate used by the serial interface. This parameter only takes effect when the serial interface is first enabled after power-up or restart.
 *
 * @param device Connected device.
 * @param baud_rate The baud rate.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_baud_rate(lw_callback_device *device, lw_grf500_baud_rate baud_rate);

/*
 * Get the I2C address of the device.
 *
 * @param device Connected device.
 * @param address The I2C address is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_i2c_address(lw_callback_device *device, uint8_t *address);

/*
 * Set the I2C address of the device. This parameter only takes effect when the serial interface is first enabled after power-up or restart.
 *
 * @param device Connected device.
 * @param address The I2C address.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_i2c_address(lw_callback_device *device, uint8_t address);

/*
 * Get the enabled state of the rolling average filter.
 *
 * @param device Connected device.
 * @param enable The rolling average filter state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_rolling_average_enable(lw_callback_device *device, lw_bool *enable);

/*
 * Set the enabled state of the rolling average filter.
 *
 * @param device Connected device.
 * @param enable The rolling average filter state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_rolling_average_enable(lw_callback_device *device, lw_bool enable);

/*
 * Get the size of the rolling average filter.
 *
 * @param device Connected device.
 * @param size The rolling average filter size is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_rolling_average_size(lw_callback_device *device, uint32_t *size);

/*
 * Set the size of the rolling average filter.
 *
 * @param device Connected device.
 * @param size The rolling average filter size.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_rolling_average_size(lw_callback_device *device, uint32_t size);

/*
 * Puts the device into sleep mode. This mode is only available in serial UART communication mode. The device is then awakened by any activity on the Serial UART communication lines and will resume previous operation.
 *
 * @param device Connected device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_sleep(lw_callback_device *device);

/*
 * Get the state of the LED.
 *
 * @param device Connected device.
 * @param enable The LED state is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_led_state(lw_callback_device *device, lw_bool *enable);

/*
 * Set the state of the LED.
 *
 * @param device Connected device.
 * @param enable The LED state.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_led_state(lw_callback_device *device, lw_bool enable);

/*
 * Get the zero offset. The zero offset is used to calibrate the distance sensor. The zero offset is added to the distance measurement.
 *
 * @param device Connected device.
 * @param offset_cm The zero offset in cm is written here.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_zero_offset(lw_callback_device *device, int32_t *offset_cm);

/*
 * Set the zero offset. The zero offset is used to calibrate the distance sensor. The zero offset is added to the distance measurement.
 *
 * @param device Connected device.
 * @param offset_cm The zero offset in cm.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_set_zero_offset(lw_callback_device *device, int32_t offset_cm);



// ----------------------------------------------------------------------------
// Fully managed helpers and composed requests.
// ----------------------------------------------------------------------------


/*
 * When communicating over the serial interface, it is important to initiate
 * serial mode. This is only required if the startup mode is set to
 * 'Wait for interface'.
 *
 * @param device Connected device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_initiate_serial(lw_callback_device *device);


/*
 * Get the all the basic product information.
 *
 * @param device Connected device.
 * @param product_info Product information.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_get_product_info(lw_callback_device *device, lw_grf500_product_info *product_info);


/*
 * Puts the device into sleep mode. This mode is only available in serial
 * UART communication mode. The device is then awakened by any activity on the
 * Serial UART communication lines and will resume previous operation.
 *
 * @param device Connected device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_sleep(lw_callback_device *device);


/*
 * Restart the device as if it had been power cycled. You will need to
 * re-establish the serial connection after this command.
 *
 * @param device Connected device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_reset(lw_callback_device *device);


/*
 * Save the current persistable device parameters.
 *
 * @param device Connected device.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure.
 */
lw_result lw_grf500_save_parameters(lw_callback_device *device);


/*
 * Get the next streamed distance data.
 *
 * If the timeout_ms is greater than 0 then this function will block until the
 * next distance data is available or the timeout is reached.
 *
 * If the timeout_ms is 0 then this function will return immediately with
 * either LW_RESULT_SUCCESS if streaming data is available, or LW_RESULT_AGAIN
 * if a response is still being built.
 *
 * @param device Connected device.
 * @param timeout_ms The timeout in milliseconds.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure, or
 *         LW_RESULT_AGAIN if the response is still building, or
 *         LW_RESULT_TIMEOUT if the timeout is reached.
 */
lw_result lw_grf500_wait_for_streamed_distance_data(lw_callback_device *device, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data, uint32_t timeout_ms);

/*
 * Get the next streamed multi signal distance data.
 *
 * If the timeout_ms is greater than 0 then this function will block until the
 * next distance data is available or the timeout is reached.
 *
 * If the timeout_ms is 0 then this function will return immediately with
 * either LW_RESULT_SUCCESS if streaming data is available, or LW_RESULT_AGAIN
 * if a response is still being built.
 *
 * @param device Connected device.
 * @param timeout_ms The timeout in milliseconds.
 * @return LW_RESULT_SUCCESS on success, or an error code on failure, or
 *         LW_RESULT_AGAIN if the response is still building, or
 *         LW_RESULT_TIMEOUT if the timeout is reached.
 */
lw_result lw_grf500_wait_for_streamed_multi_data(lw_callback_device *device, lw_grf500_multi_data *data, uint32_t timeout_ms);



// ----------------------------------------------------------------------------
// Request generators.
//
// These functions create raw requests that can be sent to the device.
//
// If you want the API to handle sending requests and receiving responses,
// with timeouts and retries, please use the managed version of these 
// commands (above).
//
// Please read the API documentation or the comments for the managed version
// of these commands (above) for more details on what each command does.
// ----------------------------------------------------------------------------
lw_result lw_grf500_create_request_read_product_name(lw_request *request);
lw_result lw_grf500_create_request_read_hardware_version(lw_request *request);
lw_result lw_grf500_create_request_read_firmware_version(lw_request *request);
lw_result lw_grf500_create_request_read_serial_number(lw_request *request);
lw_result lw_grf500_create_request_read_user_data(lw_request *request);
lw_result lw_grf500_create_request_write_user_data(lw_request *request, uint8_t *data, uint32_t data_size);
lw_result lw_grf500_create_request_read_token(lw_request *request);
lw_result lw_grf500_create_request_write_save_parameters(lw_request *request, uint16_t token);
lw_result lw_grf500_create_request_write_reset(lw_request *request, uint16_t token);
lw_result lw_grf500_create_request_read_distance_config(lw_request *request);
lw_result lw_grf500_create_request_write_distance_config(lw_request *request, lw_grf500_distance_config config);
lw_result lw_grf500_create_request_read_stream(lw_request *request);
lw_result lw_grf500_create_request_write_stream(lw_request *request, lw_grf500_stream_id stream);
lw_result lw_grf500_create_request_read_distance_data(lw_request *request);
lw_result lw_grf500_create_request_read_multi_data(lw_request *request);
lw_result lw_grf500_create_request_read_laser_firing(lw_request *request);
lw_result lw_grf500_create_request_write_laser_firing(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_temperature(lw_request *request);
lw_result lw_grf500_create_request_read_auto_exposure(lw_request *request);
lw_result lw_grf500_create_request_write_auto_exposure(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_update_rate(lw_request *request);
lw_result lw_grf500_create_request_write_update_rate(lw_request *request, float rate);
lw_result lw_grf500_create_request_read_alarm_status(lw_request *request);
lw_result lw_grf500_create_request_read_alarm_return_mode(lw_request *request);
lw_result lw_grf500_create_request_write_alarm_return_mode(lw_request *request, lw_grf500_return_mode mode);
lw_result lw_grf500_create_request_read_lost_signal_counter(lw_request *request);
lw_result lw_grf500_create_request_write_lost_signal_counter(lw_request *request, uint32_t counter);
lw_result lw_grf500_create_request_read_alarm_a_distance(lw_request *request);
lw_result lw_grf500_create_request_write_alarm_a_distance(lw_request *request, uint32_t distance_cm);
lw_result lw_grf500_create_request_read_alarm_b_distance(lw_request *request);
lw_result lw_grf500_create_request_write_alarm_b_distance(lw_request *request, uint32_t distance_cm);
lw_result lw_grf500_create_request_read_alarm_hysteresis(lw_request *request);
lw_result lw_grf500_create_request_write_alarm_hysteresis(lw_request *request, uint32_t hysteresis_cm);
lw_result lw_grf500_create_request_read_gpio_mode(lw_request *request);
lw_result lw_grf500_create_request_write_gpio_mode(lw_request *request, lw_grf500_gpio_mode mode);
lw_result lw_grf500_create_request_read_gpio_alarm_confirm_count(lw_request *request);
lw_result lw_grf500_create_request_write_gpio_alarm_confirm_count(lw_request *request, uint32_t count);
lw_result lw_grf500_create_request_read_median_filter_enable(lw_request *request);
lw_result lw_grf500_create_request_write_median_filter_enable(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_median_filter_size(lw_request *request);
lw_result lw_grf500_create_request_write_median_filter_size(lw_request *request, uint32_t size);
lw_result lw_grf500_create_request_read_smooth_filter_enable(lw_request *request);
lw_result lw_grf500_create_request_write_smooth_filter_enable(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_smooth_filter_factor(lw_request *request);
lw_result lw_grf500_create_request_write_smooth_filter_factor(lw_request *request, uint32_t factor);
lw_result lw_grf500_create_request_read_baud_rate(lw_request *request);
lw_result lw_grf500_create_request_write_baud_rate(lw_request *request, lw_grf500_baud_rate baud_rate);
lw_result lw_grf500_create_request_read_i2c_address(lw_request *request);
lw_result lw_grf500_create_request_write_i2c_address(lw_request *request, uint8_t address);
lw_result lw_grf500_create_request_read_rolling_average_enable(lw_request *request);
lw_result lw_grf500_create_request_write_rolling_average_enable(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_rolling_average_size(lw_request *request);
lw_result lw_grf500_create_request_write_rolling_average_size(lw_request *request, uint32_t size);
lw_result lw_grf500_create_request_write_sleep(lw_request *request);
lw_result lw_grf500_create_request_read_led_state(lw_request *request);
lw_result lw_grf500_create_request_write_led_state(lw_request *request, lw_bool enable);
lw_result lw_grf500_create_request_read_zero_offset(lw_request *request);
lw_result lw_grf500_create_request_write_zero_offset(lw_request *request, int32_t offset_cm);


// ----------------------------------------------------------------------------
// Response parsers.
//
// These functions extract information from responses sent by the device.
//
// If you want the API to handle sending requests and receiving responses,
// with timeouts and retries, please use the managed version of these 
// commands (above).
//
// Please read the API documentation or the comments for the managed version
// of these commands (above) for more details on what each command returns.
// ----------------------------------------------------------------------------
lw_result lw_grf500_parse_response_product_name(lw_response *response, char *product_name);
lw_result lw_grf500_parse_response_hardware_version(lw_response *response, uint32_t *hardware_version);
lw_result lw_grf500_parse_response_firmware_version(lw_response *response, lw_firmware_version *firmware_version);
lw_result lw_grf500_parse_response_serial_number(lw_response *response, char *serial_number);
lw_result lw_grf500_parse_response_user_data(lw_response *response, uint8_t *data, uint32_t data_size);
lw_result lw_grf500_parse_response_token(lw_response *response, uint16_t *token);
lw_result lw_grf500_parse_response_distance_config(lw_response *response, lw_grf500_distance_config *config);
lw_result lw_grf500_parse_response_stream(lw_response *response, lw_grf500_stream_id *stream);
lw_result lw_grf500_parse_response_distance_data(lw_response *response, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data);
lw_result lw_grf500_parse_response_multi_data(lw_response *response, lw_grf500_multi_data *data);
lw_result lw_grf500_parse_response_laser_firing(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_temperature(lw_response *response, int32_t *temperature);
lw_result lw_grf500_parse_response_auto_exposure(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_update_rate(lw_response *response, float *rate);
lw_result lw_grf500_parse_response_alarm_status(lw_response *response, lw_grf500_alarm_status *status);
lw_result lw_grf500_parse_response_alarm_return_mode(lw_response *response, lw_grf500_return_mode *mode);
lw_result lw_grf500_parse_response_lost_signal_counter(lw_response *response, uint32_t *counter);
lw_result lw_grf500_parse_response_alarm_a_distance(lw_response *response, uint32_t *distance_cm);
lw_result lw_grf500_parse_response_alarm_b_distance(lw_response *response, uint32_t *distance_cm);
lw_result lw_grf500_parse_response_alarm_hysteresis(lw_response *response, uint32_t *hysteresis_cm);
lw_result lw_grf500_parse_response_gpio_mode(lw_response *response, lw_grf500_gpio_mode *mode);
lw_result lw_grf500_parse_response_gpio_alarm_confirm_count(lw_response *response, uint32_t *count);
lw_result lw_grf500_parse_response_median_filter_enable(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_median_filter_size(lw_response *response, uint32_t *size);
lw_result lw_grf500_parse_response_smooth_filter_enable(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_smooth_filter_factor(lw_response *response, uint32_t *factor);
lw_result lw_grf500_parse_response_baud_rate(lw_response *response, lw_grf500_baud_rate *baud_rate);
lw_result lw_grf500_parse_response_i2c_address(lw_response *response, uint8_t *address);
lw_result lw_grf500_parse_response_rolling_average_enable(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_rolling_average_size(lw_response *response, uint32_t *size);
lw_result lw_grf500_parse_response_led_state(lw_response *response, lw_bool *enable);
lw_result lw_grf500_parse_response_zero_offset(lw_response *response, int32_t *offset_cm);


#ifdef __cplusplus
}
#endif

#endif // LW_API_GRF500_H
