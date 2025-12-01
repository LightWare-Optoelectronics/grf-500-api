// ----------------------------------------------------------------------------
// LightWare Serial API GRF-500
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
#include "lw_serial_api_grf500.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Fully managed request/response commands.
// ----------------------------------------------------------------------------
lw_result lw_grf500_get_product_name(lw_callback_device *device, char *product_name) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_product_name(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_product_name(&device->response, product_name);
}

lw_result lw_grf500_get_hardware_version(lw_callback_device *device, uint32_t *hardware_version) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_hardware_version(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_hardware_version(&device->response, hardware_version);
}

lw_result lw_grf500_get_firmware_version(lw_callback_device *device, lw_firmware_version *firmware_version) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_firmware_version(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_firmware_version(&device->response, firmware_version);
}

lw_result lw_grf500_get_serial_number(lw_callback_device *device, char *serial_number) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_serial_number(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_serial_number(&device->response, serial_number);
}

lw_result lw_grf500_get_user_data(lw_callback_device *device, uint8_t *data, uint32_t data_size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_user_data(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_user_data(&device->response, data, data_size);
}

lw_result lw_grf500_set_user_data(lw_callback_device *device, uint8_t *data, uint32_t data_size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_user_data(&device->request, data, data_size))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_token(lw_callback_device *device, uint16_t *token) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_token(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_token(&device->response, token);
}

lw_result lw_grf500_set_save_parameters(lw_callback_device *device, uint16_t token) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_save_parameters(&device->request, token))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_set_reset(lw_callback_device *device, uint16_t token) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_reset(&device->request, token))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_distance_config(lw_callback_device *device, lw_grf500_distance_config *config) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_distance_config(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_distance_config(&device->response, config);
}

lw_result lw_grf500_set_distance_config(lw_callback_device *device, lw_grf500_distance_config config) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_distance_config(&device->request, config))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_stream(lw_callback_device *device, lw_grf500_stream_id *stream) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_stream(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_stream(&device->response, stream);
}

lw_result lw_grf500_set_stream(lw_callback_device *device, lw_grf500_stream_id stream) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_stream(&device->request, stream))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_distance_data(lw_callback_device *device, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_distance_data(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_distance_data(&device->response, config, data);
}

lw_result lw_grf500_get_multi_data(lw_callback_device *device, lw_grf500_multi_data *data) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_multi_data(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_multi_data(&device->response, data);
}

lw_result lw_grf500_get_laser_firing(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_laser_firing(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_laser_firing(&device->response, enable);
}

lw_result lw_grf500_set_laser_firing(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_laser_firing(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_temperature(lw_callback_device *device, int32_t *temperature) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_temperature(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_temperature(&device->response, temperature);
}

lw_result lw_grf500_get_auto_exposure(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_auto_exposure(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_auto_exposure(&device->response, enable);
}

lw_result lw_grf500_set_auto_exposure(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_auto_exposure(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_update_rate(lw_callback_device *device, float *rate) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_update_rate(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_update_rate(&device->response, rate);
}

lw_result lw_grf500_set_update_rate(lw_callback_device *device, float rate) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_update_rate(&device->request, rate))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_alarm_status(lw_callback_device *device, lw_grf500_alarm_status *status) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_alarm_status(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_alarm_status(&device->response, status);
}

lw_result lw_grf500_get_alarm_return_mode(lw_callback_device *device, lw_grf500_return_mode *mode) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_alarm_return_mode(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_alarm_return_mode(&device->response, mode);
}

lw_result lw_grf500_set_alarm_return_mode(lw_callback_device *device, lw_grf500_return_mode mode) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_alarm_return_mode(&device->request, mode))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_lost_signal_counter(lw_callback_device *device, uint32_t *counter) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_lost_signal_counter(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_lost_signal_counter(&device->response, counter);
}

lw_result lw_grf500_set_lost_signal_counter(lw_callback_device *device, uint32_t counter) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_lost_signal_counter(&device->request, counter))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_alarm_a_distance(lw_callback_device *device, uint32_t *distance_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_alarm_a_distance(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_alarm_a_distance(&device->response, distance_cm);
}

lw_result lw_grf500_set_alarm_a_distance(lw_callback_device *device, uint32_t distance_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_alarm_a_distance(&device->request, distance_cm))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_alarm_b_distance(lw_callback_device *device, uint32_t *distance_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_alarm_b_distance(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_alarm_b_distance(&device->response, distance_cm);
}

lw_result lw_grf500_set_alarm_b_distance(lw_callback_device *device, uint32_t distance_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_alarm_b_distance(&device->request, distance_cm))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_alarm_hysteresis(lw_callback_device *device, uint32_t *hysteresis_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_alarm_hysteresis(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_alarm_hysteresis(&device->response, hysteresis_cm);
}

lw_result lw_grf500_set_alarm_hysteresis(lw_callback_device *device, uint32_t hysteresis_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_alarm_hysteresis(&device->request, hysteresis_cm))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_gpio_mode(lw_callback_device *device, lw_grf500_gpio_mode *mode) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_gpio_mode(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_gpio_mode(&device->response, mode);
}

lw_result lw_grf500_set_gpio_mode(lw_callback_device *device, lw_grf500_gpio_mode mode) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_gpio_mode(&device->request, mode))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_gpio_alarm_confirm_count(lw_callback_device *device, uint32_t *count) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_gpio_alarm_confirm_count(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_gpio_alarm_confirm_count(&device->response, count);
}

lw_result lw_grf500_set_gpio_alarm_confirm_count(lw_callback_device *device, uint32_t count) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_gpio_alarm_confirm_count(&device->request, count))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_median_filter_enable(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_median_filter_enable(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_median_filter_enable(&device->response, enable);
}

lw_result lw_grf500_set_median_filter_enable(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_median_filter_enable(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_median_filter_size(lw_callback_device *device, uint32_t *size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_median_filter_size(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_median_filter_size(&device->response, size);
}

lw_result lw_grf500_set_median_filter_size(lw_callback_device *device, uint32_t size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_median_filter_size(&device->request, size))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_smooth_filter_enable(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_smooth_filter_enable(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_smooth_filter_enable(&device->response, enable);
}

lw_result lw_grf500_set_smooth_filter_enable(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_smooth_filter_enable(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_smooth_filter_factor(lw_callback_device *device, uint32_t *factor) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_smooth_filter_factor(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_smooth_filter_factor(&device->response, factor);
}

lw_result lw_grf500_set_smooth_filter_factor(lw_callback_device *device, uint32_t factor) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_smooth_filter_factor(&device->request, factor))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_baud_rate(lw_callback_device *device, lw_grf500_baud_rate *baud_rate) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_baud_rate(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_baud_rate(&device->response, baud_rate);
}

lw_result lw_grf500_set_baud_rate(lw_callback_device *device, lw_grf500_baud_rate baud_rate) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_baud_rate(&device->request, baud_rate))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_i2c_address(lw_callback_device *device, uint8_t *address) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_i2c_address(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_i2c_address(&device->response, address);
}

lw_result lw_grf500_set_i2c_address(lw_callback_device *device, uint8_t address) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_i2c_address(&device->request, address))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_rolling_average_enable(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_rolling_average_enable(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_rolling_average_enable(&device->response, enable);
}

lw_result lw_grf500_set_rolling_average_enable(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_rolling_average_enable(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_rolling_average_size(lw_callback_device *device, uint32_t *size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_rolling_average_size(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_rolling_average_size(&device->response, size);
}

lw_result lw_grf500_set_rolling_average_size(lw_callback_device *device, uint32_t size) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_rolling_average_size(&device->request, size))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_set_sleep(lw_callback_device *device) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_sleep(&device->request))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_led_state(lw_callback_device *device, lw_bool *enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_led_state(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_led_state(&device->response, enable);
}

lw_result lw_grf500_set_led_state(lw_callback_device *device, lw_bool enable) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_led_state(&device->request, enable))
    return lw_send_request_get_response(device);
}

lw_result lw_grf500_get_zero_offset(lw_callback_device *device, int32_t *offset_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_read_zero_offset(&device->request))
    LW_CHECK_SUCCESS(lw_send_request_get_response(device))
    return lw_grf500_parse_response_zero_offset(&device->response, offset_cm);
}

lw_result lw_grf500_set_zero_offset(lw_callback_device *device, int32_t offset_cm) {
    LW_CHECK_SUCCESS(lw_grf500_create_request_write_zero_offset(&device->request, offset_cm))
    return lw_send_request_get_response(device);
}



// ----------------------------------------------------------------------------
// Fully managed helpers and composed requests.
// ----------------------------------------------------------------------------

lw_result lw_grf500_initiate_serial(lw_callback_device *device) {
    if (device->serial_send(device, (uint8_t *)"UUU", 3) == 0) {
        return LW_RESULT_ERROR;
    }

    return LW_RESULT_SUCCESS;
}


lw_result lw_grf500_get_product_info(lw_callback_device *device, lw_grf500_product_info *product_info) {
    LW_CHECK_SUCCESS(lw_grf500_get_product_name(device, product_info->product_name))
    LW_CHECK_SUCCESS(lw_grf500_get_hardware_version(device, &product_info->hardware_version))
    LW_CHECK_SUCCESS(lw_grf500_get_firmware_version(device, &product_info->firmware_version))
    LW_CHECK_SUCCESS(lw_grf500_get_serial_number(device, product_info->serial_number))

    return LW_RESULT_SUCCESS;
}


lw_result lw_grf500_sleep(lw_callback_device *device) {
    LW_CHECK_SUCCESS(lw_grf500_set_sleep(device))
    return LW_RESULT_SUCCESS;
}


lw_result lw_grf500_reset(lw_callback_device *device) {
    uint16_t token = 0;
    LW_CHECK_SUCCESS(lw_grf500_get_token(device, &token))
    LW_CHECK_SUCCESS(lw_grf500_set_reset(device, token))
    return LW_RESULT_SUCCESS;
}


lw_result lw_grf500_save_parameters(lw_callback_device *device) {
    uint16_t token = 0;
    LW_CHECK_SUCCESS(lw_grf500_get_token(device, &token))
    LW_CHECK_SUCCESS(lw_grf500_set_save_parameters(device, token))
    return LW_RESULT_SUCCESS;
}



lw_result lw_grf500_wait_for_streamed_distance_data(lw_callback_device *device, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data, uint32_t timeout_ms) {
    LW_CHECK_SUCCESS(lw_wait_for_next_response(device, LW_GRF500_COMMAND_DISTANCE_DATA, timeout_ms))
    return lw_grf500_parse_response_distance_data(&device->response, config, data);
}

lw_result lw_grf500_wait_for_streamed_multi_data(lw_callback_device *device, lw_grf500_multi_data *data, uint32_t timeout_ms) {
    LW_CHECK_SUCCESS(lw_wait_for_next_response(device, LW_GRF500_COMMAND_MULTI_DATA, timeout_ms))
    return lw_grf500_parse_response_multi_data(&device->response, data);
}



// ----------------------------------------------------------------------------
// Request generators.
// ----------------------------------------------------------------------------
lw_result lw_grf500_create_request_read_product_name(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_PRODUCT_NAME);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_hardware_version(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_HARDWARE_VERSION);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_firmware_version(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_FIRMWARE_VERSION);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_serial_number(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_SERIAL_NUMBER);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_user_data(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_USER_DATA);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_user_data(lw_request *request, uint8_t *data, uint32_t data_size) {
    if (data_size != 16) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_data(request, LW_GRF500_COMMAND_USER_DATA, data, data_size);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_token(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_TOKEN);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_save_parameters(lw_request *request, uint16_t token) {
    lw_create_request_write_uint16(request, LW_GRF500_COMMAND_SAVE_PARAMETERS, token);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_reset(lw_request *request, uint16_t token) {
    lw_create_request_write_uint16(request, LW_GRF500_COMMAND_RESET, token);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_distance_config(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_DISTANCE_CONFIG);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_distance_config(lw_request *request, lw_grf500_distance_config config) {
    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_DISTANCE_CONFIG, (uint32_t)config);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_stream(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_STREAM);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_stream(lw_request *request, lw_grf500_stream_id stream) {
    if (stream != LW_GRF500_STREAM_ID_NONE &&
        stream != LW_GRF500_STREAM_ID_DISTANCE_DATA &&
        stream != LW_GRF500_STREAM_ID_MULTI_DATA) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_STREAM, (uint32_t)stream);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_distance_data(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_DISTANCE_DATA);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_multi_data(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_MULTI_DATA);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_laser_firing(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_LASER_FIRING);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_laser_firing(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_LASER_FIRING, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_temperature(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_TEMPERATURE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_auto_exposure(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_AUTO_EXPOSURE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_auto_exposure(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_AUTO_EXPOSURE, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_update_rate(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_UPDATE_RATE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_update_rate(lw_request *request, float rate) {
    if (rate < (float)0.5 || rate > (float)10) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_UPDATE_RATE, (uint32_t)(rate* 10));
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_alarm_status(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ALARM_STATUS);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_alarm_return_mode(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ALARM_RETURN_MODE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_alarm_return_mode(lw_request *request, lw_grf500_return_mode mode) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_ALARM_RETURN_MODE, (uint8_t)mode);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_lost_signal_counter(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_LOST_SIGNAL_COUNTER);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_lost_signal_counter(lw_request *request, uint32_t counter) {
    if (counter < 1 || counter > 250) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_LOST_SIGNAL_COUNTER, counter);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_alarm_a_distance(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ALARM_A_DISTANCE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_alarm_a_distance(lw_request *request, uint32_t distance_cm) {
    if (distance_cm > 30000) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_ALARM_A_DISTANCE, (distance_cm/ 10));
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_alarm_b_distance(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ALARM_B_DISTANCE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_alarm_b_distance(lw_request *request, uint32_t distance_cm) {
    if (distance_cm > 30000) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_ALARM_B_DISTANCE, (distance_cm/ 10));
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_alarm_hysteresis(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ALARM_HYSTERESIS);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_alarm_hysteresis(lw_request *request, uint32_t hysteresis_cm) {
    if (hysteresis_cm > 3000) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_ALARM_HYSTERESIS, (hysteresis_cm/ 10));
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_gpio_mode(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_GPIO_MODE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_gpio_mode(lw_request *request, lw_grf500_gpio_mode mode) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_GPIO_MODE, (uint8_t)mode);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_gpio_alarm_confirm_count(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_GPIO_ALARM_CONFIRM_COUNT);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_gpio_alarm_confirm_count(lw_request *request, uint32_t count) {
    if (count > 1000) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_GPIO_ALARM_CONFIRM_COUNT, count);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_median_filter_enable(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_MEDIAN_FILTER_ENABLE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_median_filter_enable(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_MEDIAN_FILTER_ENABLE, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_median_filter_size(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_MEDIAN_FILTER_SIZE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_median_filter_size(lw_request *request, uint32_t size) {
    if (size < 3 || size > 32) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_MEDIAN_FILTER_SIZE, size);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_smooth_filter_enable(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_SMOOTH_FILTER_ENABLE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_smooth_filter_enable(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_SMOOTH_FILTER_ENABLE, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_smooth_filter_factor(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_SMOOTH_FILTER_FACTOR);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_smooth_filter_factor(lw_request *request, uint32_t factor) {
    if (factor < 1 || factor > 99) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_SMOOTH_FILTER_FACTOR, factor);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_baud_rate(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_BAUD_RATE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_baud_rate(lw_request *request, lw_grf500_baud_rate baud_rate) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_BAUD_RATE, (uint8_t)baud_rate);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_i2c_address(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_I2C_ADDRESS);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_i2c_address(lw_request *request, uint8_t address) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_I2C_ADDRESS, address);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_rolling_average_enable(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ROLLING_AVERAGE_ENABLE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_rolling_average_enable(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_ROLLING_AVERAGE_ENABLE, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_rolling_average_size(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ROLLING_AVERAGE_SIZE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_rolling_average_size(lw_request *request, uint32_t size) {
    if (size < 2 || size > 32) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    lw_create_request_write_uint32(request, LW_GRF500_COMMAND_ROLLING_AVERAGE_SIZE, size);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_sleep(lw_request *request) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_SLEEP, 123);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_led_state(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_LED_STATE);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_led_state(lw_request *request, lw_bool enable) {
    lw_create_request_write_uint8(request, LW_GRF500_COMMAND_LED_STATE, (uint8_t)enable);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_read_zero_offset(lw_request *request) {
    lw_create_request_read(request, LW_GRF500_COMMAND_ZERO_OFFSET);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_create_request_write_zero_offset(lw_request *request, int32_t offset_cm) {
    lw_create_request_write_int32(request, LW_GRF500_COMMAND_ZERO_OFFSET, (offset_cm/ 10));
    return LW_RESULT_SUCCESS;
}



// ----------------------------------------------------------------------------
// Response parsers.
// ----------------------------------------------------------------------------
lw_result lw_grf500_parse_response_product_name(lw_response *response, char *product_name) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_PRODUCT_NAME)
    lw_parse_response_string(response, product_name, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_hardware_version(lw_response *response, uint32_t *hardware_version) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_HARDWARE_VERSION)
    lw_parse_response_uint32(response, hardware_version, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_firmware_version(lw_response *response, lw_firmware_version *firmware_version) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_FIRMWARE_VERSION)
    uint32_t firmware_version_int;
    lw_parse_response_uint32(response, &firmware_version_int, 0);
    *firmware_version = lw_expand_firmware_version(firmware_version_int);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_serial_number(lw_response *response, char *serial_number) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_SERIAL_NUMBER)
    lw_parse_response_string(response, serial_number, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_user_data(lw_response *response, uint8_t *data, uint32_t data_size) {
    if (data_size > 16) {
        return LW_RESULT_INVALID_PARAMETER;
    }

    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_USER_DATA)
    lw_parse_response_data(response, data, data_size, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_token(lw_response *response, uint16_t *token) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_TOKEN)
    lw_parse_response_uint16(response, token, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_distance_config(lw_response *response, lw_grf500_distance_config *config) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_DISTANCE_CONFIG)
    uint32_t temp_value;
    lw_parse_response_uint32(response, &temp_value, 0);
    *config = (lw_grf500_distance_config)temp_value;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_stream(lw_response *response, lw_grf500_stream_id *stream) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_STREAM)
    uint32_t value = 0;
    lw_parse_response_uint32(response, &value, 0);
    *stream = (lw_grf500_stream_id)value;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_distance_data(lw_response *response, lw_grf500_distance_config config, lw_grf500_distance_data_cm *data) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_DISTANCE_DATA)
    uint32_t offset = 0;

    if (config & LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_RAW) {
        lw_parse_response_int32(response, &data->first_return_raw_cm, offset);
        data->first_return_raw_cm = data->first_return_raw_cm * 10;
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_FILTERED) {
        lw_parse_response_int32(response, &data->first_return_filtered_cm, offset);
        data->first_return_filtered_cm = data->first_return_filtered_cm * 10;
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_FIRST_RETURN_STRENGTH) {
        lw_parse_response_int32(response, &data->first_return_strength, offset);
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_RAW) {
        lw_parse_response_int32(response, &data->last_return_raw_cm, offset);
        data->last_return_raw_cm = data->last_return_raw_cm * 10;
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_FILTERED) {
        lw_parse_response_int32(response, &data->last_return_filtered_cm, offset);
        data->last_return_filtered_cm = data->last_return_filtered_cm * 10;
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_LAST_RETURN_STRENGTH) {
        lw_parse_response_int32(response, &data->last_return_strength, offset);
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_TEMPERATURE) {
        lw_parse_response_int32(response, &data->temperature, offset);
        offset += sizeof(int32_t);
    }

    if (config & LW_GRF500_DISTANCE_CONFIG_ALARM_STATUS) {
        lw_parse_response_int32(response, &data->alarm_status, offset);
        offset += sizeof(int32_t);
    }

    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_multi_data(lw_response *response, lw_grf500_multi_data *data) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_MULTI_DATA)
    uint32_t offset = 0;

for (int i = 0; i < 5; ++i) {
	lw_parse_response_int32(response, &data->signals[i].distance_mm, offset);
	offset += sizeof(int32_t);
	lw_parse_response_int32(response, &data->signals[i].strength, offset);
	offset += sizeof(int32_t);
}

lw_parse_response_int32(response, &data->temperature, offset);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_laser_firing(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_LASER_FIRING)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_temperature(lw_response *response, int32_t *temperature) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_TEMPERATURE)
    lw_parse_response_int32(response, temperature, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_auto_exposure(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_AUTO_EXPOSURE)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_update_rate(lw_response *response, float *rate) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_UPDATE_RATE)
    uint32_t temp_value;
    lw_parse_response_uint32(response, &temp_value, 0);
    *rate = (float)temp_value;
    *rate = (*rate) / 10;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_alarm_status(lw_response *response, lw_grf500_alarm_status *status) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ALARM_STATUS)
    uint32_t alarm_status = 0;
lw_parse_response_uint32(response, &alarm_status, 0);

status->alarm_a = (alarm_status >> 0) & 0xFF;
status->alarm_b = (alarm_status >> 8) & 0xFF;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_alarm_return_mode(lw_response *response, lw_grf500_return_mode *mode) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ALARM_RETURN_MODE)
    uint8_t temp_value;
    lw_parse_response_uint8(response, &temp_value, 0);
    *mode = (lw_grf500_return_mode)temp_value;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_lost_signal_counter(lw_response *response, uint32_t *counter) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_LOST_SIGNAL_COUNTER)
    lw_parse_response_uint32(response, counter, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_alarm_a_distance(lw_response *response, uint32_t *distance_cm) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ALARM_A_DISTANCE)
    lw_parse_response_uint32(response, distance_cm, 0);
    *distance_cm = (*distance_cm) * 10;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_alarm_b_distance(lw_response *response, uint32_t *distance_cm) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ALARM_B_DISTANCE)
    lw_parse_response_uint32(response, distance_cm, 0);
    *distance_cm = (*distance_cm) * 10;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_alarm_hysteresis(lw_response *response, uint32_t *hysteresis_cm) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ALARM_HYSTERESIS)
    lw_parse_response_uint32(response, hysteresis_cm, 0);
    *hysteresis_cm = (*hysteresis_cm) * 10;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_gpio_mode(lw_response *response, lw_grf500_gpio_mode *mode) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_GPIO_MODE)
    uint8_t temp_value;
    lw_parse_response_uint8(response, &temp_value, 0);
    *mode = (lw_grf500_gpio_mode)temp_value;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_gpio_alarm_confirm_count(lw_response *response, uint32_t *count) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_GPIO_ALARM_CONFIRM_COUNT)
    lw_parse_response_uint32(response, count, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_median_filter_enable(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_MEDIAN_FILTER_ENABLE)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_median_filter_size(lw_response *response, uint32_t *size) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_MEDIAN_FILTER_SIZE)
    lw_parse_response_uint32(response, size, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_smooth_filter_enable(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_SMOOTH_FILTER_ENABLE)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_smooth_filter_factor(lw_response *response, uint32_t *factor) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_SMOOTH_FILTER_FACTOR)
    lw_parse_response_uint32(response, factor, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_baud_rate(lw_response *response, lw_grf500_baud_rate *baud_rate) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_BAUD_RATE)
    uint8_t temp_value;
    lw_parse_response_uint8(response, &temp_value, 0);
    *baud_rate = (lw_grf500_baud_rate)temp_value;
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_i2c_address(lw_response *response, uint8_t *address) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_I2C_ADDRESS)
    lw_parse_response_uint8(response, address, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_rolling_average_enable(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ROLLING_AVERAGE_ENABLE)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_rolling_average_size(lw_response *response, uint32_t *size) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ROLLING_AVERAGE_SIZE)
    lw_parse_response_uint32(response, size, 0);
    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_led_state(lw_response *response, lw_bool *enable) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_LED_STATE)
    lw_parse_response_uint8(response, (uint8_t*)enable, 0);    return LW_RESULT_SUCCESS;
}

lw_result lw_grf500_parse_response_zero_offset(lw_response *response, int32_t *offset_cm) {
    LW_CHECK_COMMAND_ID(response, LW_GRF500_COMMAND_ZERO_OFFSET)
    lw_parse_response_int32(response, offset_cm, 0);
    *offset_cm = (*offset_cm) * 10;
    return LW_RESULT_SUCCESS;
}


