"""
LightWare Serial API GRF-500
Version: 1.1.0
Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
https://www.lightwarelidar.com

This module provides a Python interface to the GRF-500 sensor over the serial interfaces (UART/USB).

License: MIT No Attribution (MIT-0)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
"""

from typing import NamedTuple
from enum import Enum, IntFlag
import time
import serial

# -----------------------------------------------------------------------------
# Serial service handler.
# -----------------------------------------------------------------------------
class SerialServiceHandler:
    """
    Provides a serial port implementation to communicate with a physical device.
    """

    def __init__(self, port_name: str, baud_rate: int):
        self.port = serial.Serial(port_name, baud_rate)

    def __del__(self):
        self.port.close()

    def read_byte(self) -> int | None:
        """
        Read a single byte from the serial port and return it as an integer.
        Returns None if no data is available.
        """

        # This should be a non-blocking read.
        if self.port.in_waiting == 0:
            return None
        else:
            return ord(self.port.read(1))

    def write(self, data):
        """Write data to the serial port."""

        self.port.write(data)


# -----------------------------------------------------------------------------
# Helpers.
# -----------------------------------------------------------------------------
def create_crc(data):
    """Creates a 16-bit CRC of the specified data."""

    crc = 0

    for i in data:
        code = crc >> 8
        code ^= int(i)
        code ^= code >> 4
        crc = crc << 8
        crc ^= code
        code = code << 5
        crc ^= code
        code = code << 7
        crc ^= code
        crc &= 0xFFFF

    return crc


# -----------------------------------------------------------------------------
# Request.
# -----------------------------------------------------------------------------
class Request:
    """Handles the creation of request packets."""

    command_id: int
    data: bytearray

    def __init__(self):
        self.command_id = None
        self.data = []

    def build_packet(self, command_id: int, write: bool, data: list[int] = None):
        if data is None:
            data = []

        self.command_id = command_id

        payload_length = 1 + len(data)
        flags = payload_length << 6

        if write:
            flags = flags | 0x1

        packet_bytes = [0xAA, flags & 0xFF, (flags >> 8) & 0xFF, command_id]
        packet_bytes.extend(data)
        crc = create_crc(packet_bytes)
        packet_bytes.append(crc & 0xFF)
        packet_bytes.append((crc >> 8) & 0xFF)

        self.data = bytearray(packet_bytes)

    def create_read(self, command_id: int):
        self.build_packet(command_id, False)

    def create_write_int8(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 1, byteorder="little", signed=True)
        )

    def create_write_int16(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 2, byteorder="little", signed=True)
        )

    def create_write_int32(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 4, byteorder="little", signed=True)
        )

    def create_write_uint8(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 1, byteorder="little", signed=False)
        )

    def create_write_uint16(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 2, byteorder="little", signed=False)
        )

    def create_write_uint32(self, command_id: int, value: int):
        self.build_packet(
            command_id, True, int.to_bytes(value, 4, byteorder="little", signed=False)
        )

    def create_write_data(self, command_id: int, data: list[int]):
        self.build_packet(command_id, True, data)


# -----------------------------------------------------------------------------
# Response.
# -----------------------------------------------------------------------------
class Response:
    """Handles the parsing of response packets."""

    command_id: int
    data: bytearray
    parse_state: int
    payload_size: int

    def __init__(self):
        self.reset()

    def reset(self):
        self.command_id = None
        self.data = []
        self.parse_state = 0
        self.payload_size = 0

    def feed(self, byte: int):
        if self.parse_state == 0:
            if byte == 0xAA:
                self.parse_state = 1
                self.data = [0xAA]

        elif self.parse_state == 1:
            self.parse_state = 2
            self.data.append(byte)

        elif self.parse_state == 2:
            self.parse_state = 3
            self.data.append(byte)
            self.payload_size = (self.data[1] | (self.data[2] << 8)) >> 6
            self.payload_size += 2

            if self.payload_size > 1019 or self.payload_size < 3:
                self.parse_state = 0

        elif self.parse_state == 3:
            self.data.append(byte)
            self.payload_size -= 1

            if self.payload_size == 0:
                self.parse_state = 0
                crc = self.data[len(self.data) - 2] | (
                    self.data[len(self.data) - 1] << 8
                )
                verify_crc = create_crc(self.data[0:-2])

                if crc == verify_crc:
                    self.command_id = self.data[3]
                    return True

        return False

    def parse_int8(self):
        return int.from_bytes(self.data[4:5], byteorder="little", signed=True)

    def parse_int16(self):
        return int.from_bytes(self.data[4:6], byteorder="little", signed=True)

    def parse_int32(self):
        return int.from_bytes(self.data[4:8], byteorder="little", signed=True)

    def parse_uint8(self):
        return int.from_bytes(self.data[4:5], byteorder="little", signed=False)

    def parse_uint16(self):
        return int.from_bytes(self.data[4:6], byteorder="little", signed=False)

    def parse_uint32(self):
        return int.from_bytes(self.data[4:8], byteorder="little", signed=False)

    def parse_string(self):
        str16 = ""
        for i in range(0, 16):
            if self.data[4 + i] == 0:
                break
            else:
                str16 += chr(self.data[4 + i])

        return str16

    def parse_data(self, size):
        return self.data[4 : 4 + size]


# -----------------------------------------------------------------------------
# GRF-500
# -----------------------------------------------------------------------------
class CommandId(Enum):
    PRODUCT_NAME = 0
    HARDWARE_VERSION = 1
    FIRMWARE_VERSION = 2
    SERIAL_NUMBER = 3
    USER_DATA = 9
    TOKEN = 10
    SAVE_PARAMETERS = 12
    RESET = 14
    DISTANCE_CONFIG = 27
    STREAM = 30
    DISTANCE_DATA = 44
    MULTI_DATA = 45
    LASER_FIRING = 50
    TEMPERATURE = 55
    AUTO_EXPOSURE = 70
    UPDATE_RATE = 74
    ALARM_STATUS = 76
    ALARM_RETURN_MODE = 77
    LOST_SIGNAL_COUNTER = 78
    ALARM_A_DISTANCE = 79
    ALARM_B_DISTANCE = 80
    ALARM_HYSTERESIS = 81
    GPIO_MODE = 83
    GPIO_ALARM_CONFIRM_COUNT = 84
    MEDIAN_FILTER_ENABLE = 86
    MEDIAN_FILTER_SIZE = 87
    SMOOTH_FILTER_ENABLE = 88
    SMOOTH_FILTER_FACTOR = 89
    BAUD_RATE = 91
    I2C_ADDRESS = 92
    ROLLING_AVERAGE_ENABLE = 93
    ROLLING_AVERAGE_SIZE = 94
    SLEEP = 98
    LED_STATE = 110
    ZERO_OFFSET = 114


class StreamId(Enum):
    NONE = 0
    DISTANCE_DATA = 5
    MULTI_DATA = 6


class ProductInformation(NamedTuple):
    product_name: str
    hardware_version: int
    firmware_version: str
    serial_number: str

class DistanceConfig(IntFlag):
    FIRST_RETURN_RAW = 1 << 0
    FIRST_RETURN_FILTERED = 1 << 1
    FIRST_RETURN_STRENGTH = 1 << 2
    LAST_RETURN_RAW = 1 << 3
    LAST_RETURN_FILTERED = 1 << 4
    LAST_RETURN_STRENGTH = 1 << 5
    TEMPERATURE = 1 << 6
    ALARM_STATUS = 1 << 7
    ALL = 0xFF

class ReturnMode(Enum):
    """The closest or furthest detected target in a single measurement."""

    FIRST = 0
    LAST = 1

class BaudRate(Enum):
    """The baud rate for the serial interface."""

    BAUD_9600 = 0
    BAUD_19200 = 1
    BAUD_38400 = 2
    BAUD_57600 = 3
    BAUD_115200 = 4
    BAUD_230400 = 5
    BAUD_460800 = 6
    BAUD_921600 = 7

class AlarmStatus:
	def __init__(self, response: Response):
		data = response.parse_data(2)
		self.alarm_a = data[0]
		self.alarm_b = data[1]


class GpioMode(Enum):
    """Operational function for the alarm pin on the external connector."""

    NO_OUTPUT = 0
    """Output of 0 V."""
    ALARM_A = 1
    """Output of 3.3 V when alarm A is active."""
    ALARM_B = 2
    """Output of 3.3 V when alarm B is active."""

class DistanceDataCm:
    @staticmethod
    def get_size(config: DistanceConfig) -> int:
        return config.bit_count() * 4

    def __init__(self, response: Response, config: DistanceConfig):
        self.first_return_raw_cm = None
        self.first_return_filtered_cm = None
        self.first_return_strength = None
        self.last_return_raw_cm = None
        self.last_return_filtered_cm = None
        self.last_return_strength = None
        self.temperature = None
        self.alarm_status = None

        num_results = config.bit_count()
        data = response.parse_data(4 * num_results)
        offset = 0

        if config & DistanceConfig.FIRST_RETURN_RAW:
            self.first_return_raw_cm = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)* 10
            offset += 4

        if config & DistanceConfig.FIRST_RETURN_FILTERED:
            self.first_return_filtered_cm = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)* 10
            offset += 4

        if config & DistanceConfig.FIRST_RETURN_STRENGTH:
            self.first_return_strength = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
            offset += 4

        if config & DistanceConfig.LAST_RETURN_RAW:
            self.last_return_raw_cm = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)* 10
            offset += 4

        if config & DistanceConfig.LAST_RETURN_FILTERED:
            self.last_return_filtered_cm = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)* 10
            offset += 4

        if config & DistanceConfig.LAST_RETURN_STRENGTH:
            self.last_return_strength = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
            offset += 4

        if config & DistanceConfig.TEMPERATURE:
            self.temperature = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
            offset += 4

        if config & DistanceConfig.ALARM_STATUS:
            self.alarm_status = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
            offset += 4

class MultiData:
	def __init__(self, response: Response):
		self.distance_mm = [0] * 5
		self.strength = [0] * 5
		self.temperature = 0

		data = response.parse_data(44)
		offset = 0

		for i in range(0, 5):
			self.distance_mm[i] = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
			offset += 4
			self.strength[i] = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)
			offset += 4

		self.temperature = int.from_bytes(data[offset : offset + 4], byteorder="little", signed=True)





class Grf500:
    """
    Handles request/response communication with the sensor.
    """

    def __init__(self, service_handler: SerialServiceHandler):
        self.service_handler = service_handler
        self.request = Request()
        self.response = Response()
        self.request_retries = 4
        self.request_timeout = 1.0

    # -----------------------------------------------------------------------------
    # Communication functions.
    # -----------------------------------------------------------------------------
    def wait_for_next_response(self, command: int, timeout: float = 1) -> bool:
        """
        Wait for the next response from the device.

        :param command: The expected command ID.
        :param timeout: The maximum time to wait for a response, if 0 then non-blocking.
        :return: True if the response matches the expected command ID. False if the timeout is
        reached, or no bytes were read if non-blocking.
        """

        end_time = time.time() + timeout

        while True:
            byte = self.service_handler.read_byte()

            if byte is None:
                if time.time() >= end_time:
                    return False
            else:
                if self.response.feed(byte):
                    if self.response.command_id == command:
                        return True

    def send_request_get_response(self, timeout=1):
        retries = self.request_retries

        while retries > 0:
            retries -= 1

            self.service_handler.write(self.request.data)

            self.response.reset()

            response = self.wait_for_next_response(self.request.command_id, timeout)
            if response is True:
                return

        raise TimeoutError("Request failed to get a response.")

    # -----------------------------------------------------------------------------
    # Device commands.
    # -----------------------------------------------------------------------------
    def get_product_name(self) -> str:
        """
        Get the product name.

        :return: The product name.
        """

        self.request.create_read(CommandId.PRODUCT_NAME.value)
        self.send_request_get_response()
        return self.response.parse_string()

    def get_hardware_version(self) -> int:
        """
        Get the hardware version.

        :return: The hardware version.
        """

        self.request.create_read(CommandId.HARDWARE_VERSION.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def get_firmware_version(self) -> str:
        """
        Get the firmware version.

        :return: The firmware version.
        """

        self.request.create_read(CommandId.FIRMWARE_VERSION.value)
        self.send_request_get_response()
        firmware_bytes = self.response.parse_data(3)
        firmware_str = f"{firmware_bytes[2]}.{firmware_bytes[1]}.{firmware_bytes[0]}"
        return firmware_str

    def get_serial_number(self) -> str:
        """
        Get the serial number.

        :return: The serial number.
        """

        self.request.create_read(CommandId.SERIAL_NUMBER.value)
        self.send_request_get_response()
        return self.response.parse_string()

    def get_user_data(self) -> list[int]:
        """
        Get the user data.

        :return: 16-bytes of user data.
        """

        self.request.create_read(CommandId.USER_DATA.value)
        self.send_request_get_response()
        return self.response.parse_data(16)

    def set_user_data(self, data: list[int]):
        """
        Write 16-bytes of user data to the device.

        :param data: A 16-byte buffer of user data.
        """

        if len(data) != 16:
            raise ValueError("Data must be 16 bytes.")

        self.request.create_write_data(CommandId.USER_DATA.value, data)
        self.send_request_get_response()

    def get_token(self) -> int:
        """
        Get the next usable safety token for saving parameters or resetting the device.

        :return: The next usable safety token.
        """

        self.request.create_read(CommandId.TOKEN.value)
        self.send_request_get_response()
        return self.response.parse_uint16()

    def set_save_parameters(self, token: int):
        """
        Save the current persistable device parameters.

        :param token: The latest unused safety token
        """

        self.request.create_write_uint16(CommandId.SAVE_PARAMETERS.value, token)
        self.send_request_get_response()

    def set_reset(self, token: int):
        """
        Restart the device as if it had been power cycled.

        :param token: The latest unused safety token.
        """

        self.request.create_write_uint16(CommandId.RESET.value, token)
        self.send_request_get_response()

    def get_distance_config(self) -> DistanceConfig:
        """
        Get the distance configuration flags. Used to determine which data will be available for the distance data command. See DistanceConfig for flag definitions.

        :return: The current distance configuration.
        """

        self.request.create_read(CommandId.DISTANCE_CONFIG.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_distance_config(self, config: DistanceConfig):
        """
        Set the distance configuration flags. Used to determine which data will be available for the distance data command. See DistanceConfig for flag definitions.

        :param config: The configuration configuration flags.
        """

        self.request.create_write_uint32(CommandId.DISTANCE_CONFIG.value, config.value)
        self.send_request_get_response()

    def get_stream(self) -> StreamId:
        """
        Get the currently set stream.

        :return: The current stream.
        """

        self.request.create_read(CommandId.STREAM.value)
        self.send_request_get_response()
        return StreamId(self.response.parse_uint32())

    def set_stream(self, stream: StreamId):
        """
        Set the stream.

        :param stream: The stream to set.
        """

        self.request.create_write_uint32(CommandId.STREAM.value, stream.value)
        self.send_request_get_response()

    def get_distance_data(self, config: DistanceConfig) -> DistanceDataCm:
        """
        Get the distance data. Only values as defined by the distance configuration will be available.

        :param config: The distance configuration flags.

        :return: Distance data or None if the timeout is reached.
        """

        self.request.create_read(CommandId.DISTANCE_DATA.value)
        self.send_request_get_response()
        return DistanceDataCm(self.response, config)

    def get_multi_data(self) -> MultiData:
        """
        Get 5 signals from the device. Each signal contains a distance and strength. If more than one target is identified within its field of view, the device will report up to five discrete target distances.

        :return: Multi data or None if the timeout is reached.
        """

        self.request.create_read(CommandId.MULTI_DATA.value)
        self.send_request_get_response()
        return MultiData(self.response)

    def get_laser_firing(self) -> bool:
        """
        Get the laser firing state.

        :return: The laser firing state.
        """

        self.request.create_read(CommandId.LASER_FIRING.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_laser_firing(self, enable: bool):
        """
        Set the laser firing state.

        :param enable: The laser firing state.
        """

        self.request.create_write_uint8(CommandId.LASER_FIRING.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_temperature(self) -> int:
        """
        Get the temperature.

        :return: The temperature in 100th of a degree Celsius.
        """

        self.request.create_read(CommandId.TEMPERATURE.value)
        self.send_request_get_response()
        return self.response.parse_int32()

    def get_auto_exposure(self) -> bool:
        """
        Get the auto exposure state.

        :return: The auto exposure state.
        """

        self.request.create_read(CommandId.AUTO_EXPOSURE.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_auto_exposure(self, enable: bool):
        """
        Set the auto exposure state.

        :param enable: The auto exposure state.
        """

        self.request.create_write_uint8(CommandId.AUTO_EXPOSURE.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_update_rate(self) -> float:
        """
        Get the update rate.

        :return: The update rate in Hz.
        """

        self.request.create_read(CommandId.UPDATE_RATE.value)
        self.send_request_get_response()
        return self.response.parse_uint32()/ 10

    def set_update_rate(self, rate: float):
        """
        Set the update rate.

        :param rate: The update rate in Hz.
        """

        if rate < 0.5 or rate > 10:
            raise ValueError("rate must be from 0.5 to 10.")

        self.request.create_write_uint32(CommandId.UPDATE_RATE.value, rate* 10)
        self.send_request_get_response()

    def get_alarm_status(self) -> AlarmStatus:
        """
        Get the alarm status. A value of 0 means the alarm is not active. A value of 1 means the alarm is active.

        :return: The alarm status of Alarm A and Alarm B.
        """

        self.request.create_read(CommandId.ALARM_STATUS.value)
        self.send_request_get_response()
        return AlarmStatus(self.response)

    def get_alarm_return_mode(self) -> ReturnMode:
        """
        Get the return mode used for alarms.

        :return: The alarm return mode.
        """

        self.request.create_read(CommandId.ALARM_RETURN_MODE.value)
        self.send_request_get_response()
        return self.response.parse_uint8()

    def set_alarm_return_mode(self, mode: ReturnMode):
        """
        Set the return mode used for alarms.

        :param mode: The alarm return mode.
        """

        self.request.create_write_uint8(CommandId.ALARM_RETURN_MODE.value, mode)
        self.send_request_get_response()

    def get_lost_signal_counter(self) -> int:
        """
        Get the number of lost signal returns before a lost signal indication is output on the distance value.
Lost signal value will be -1000 mm.

        :return: The lost signal counter.
        """

        self.request.create_read(CommandId.LOST_SIGNAL_COUNTER.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_lost_signal_counter(self, counter: int):
        """
        Set the number of lost signal returns before a lost signal indication is output on the distance value. Lost signal value will be -1000 mm.

        :param counter: The lost signal counter.
        """

        if counter < 1 or counter > 250:
            raise ValueError("counter must be from 1 to 250.")

        self.request.create_write_uint32(CommandId.LOST_SIGNAL_COUNTER.value, counter)
        self.send_request_get_response()

    def get_alarm_a_distance(self) -> int:
        """
        Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.

        :return: The alarm A distance in cm.
        """

        self.request.create_read(CommandId.ALARM_A_DISTANCE.value)
        self.send_request_get_response()
        return self.response.parse_uint32()* 10

    def set_alarm_a_distance(self, distance_cm: int):
        """
        Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.

        :param distance_cm: The alarm A distance in cm.
        """

        if distance_cm < 0 or distance_cm > 30000:
            raise ValueError("distance_cm must be from 0 to 30000.")

        self.request.create_write_uint32(CommandId.ALARM_A_DISTANCE.value, distance_cm/ 10)
        self.send_request_get_response()

    def get_alarm_b_distance(self) -> int:
        """
        Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.

        :return: The alarm B distance in cm.
        """

        self.request.create_read(CommandId.ALARM_B_DISTANCE.value)
        self.send_request_get_response()
        return self.response.parse_uint32()* 10

    def set_alarm_b_distance(self, distance_cm: int):
        """
        Any distance measured shorter than the set distance will activate this alarm. The alarm will reset when the distance returns to beyond the set distance plus the alarm hysteresis setting.

        :param distance_cm: The alarm B distance in cm.
        """

        if distance_cm < 0 or distance_cm > 30000:
            raise ValueError("distance_cm must be from 0 to 30000.")

        self.request.create_write_uint32(CommandId.ALARM_B_DISTANCE.value, distance_cm/ 10)
        self.send_request_get_response()

    def get_alarm_hysteresis(self) -> int:
        """
        The hysteresis setting is used to prevent the alarm from toggling on and off when the distance is near the alarm set point.

        :return: The alarm hysterisis in cm.
        """

        self.request.create_read(CommandId.ALARM_HYSTERESIS.value)
        self.send_request_get_response()
        return self.response.parse_uint32()* 10

    def set_alarm_hysteresis(self, hysteresis_cm: int):
        """
        The hysteresis setting is used to prevent the alarm from toggling on and off when the distance is near the alarm set point.

        :param hysteresis_cm: The alarm hysteresis in cm.
        """

        if hysteresis_cm < 0 or hysteresis_cm > 3000:
            raise ValueError("hysteresis_cm must be from 0 to 3000.")

        self.request.create_write_uint32(CommandId.ALARM_HYSTERESIS.value, hysteresis_cm/ 10)
        self.send_request_get_response()

    def get_gpio_mode(self) -> GpioMode:
        """
        Get the output mode of the alarm pin on the external connector. See GpioMode for mode definitions.

        :return: The alarm pin output mode.
        """

        self.request.create_read(CommandId.GPIO_MODE.value)
        self.send_request_get_response()
        return self.response.parse_uint8()

    def set_gpio_mode(self, mode: GpioMode):
        """
        Set the output mode of the alarm pin on the external connector. See GpioMode for mode definitions.

        :param mode: The alarm pin output mode.
        """

        self.request.create_write_uint8(CommandId.GPIO_MODE.value, mode)
        self.send_request_get_response()

    def get_gpio_alarm_confirm_count(self) -> int:
        """
        Get the number of confirmed alarm signals before the alarm pin on the external connector is activated. When the alarm resets the pin is immediately deactivated.

        :return: The number of confirmations.
        """

        self.request.create_read(CommandId.GPIO_ALARM_CONFIRM_COUNT.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_gpio_alarm_confirm_count(self, count: int):
        """
        Set the number of confirmed alarm signals before the alarm pin on the external connector is activated. When the alarm resets the pin is immediately deactivated.

        :param count: Number of confirmations.
        """

        if count < 0 or count > 1000:
            raise ValueError("count must be from 0 to 1000.")

        self.request.create_write_uint32(CommandId.GPIO_ALARM_CONFIRM_COUNT.value, count)
        self.send_request_get_response()

    def get_median_filter_enable(self) -> bool:
        """
        Get the enabled state of the median filter.

        :return: The median filter state.
        """

        self.request.create_read(CommandId.MEDIAN_FILTER_ENABLE.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_median_filter_enable(self, enable: bool):
        """
        Set the enabled state of the median filter.

        :param enable: The median filter state.
        """

        self.request.create_write_uint8(CommandId.MEDIAN_FILTER_ENABLE.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_median_filter_size(self) -> int:
        """
        Get the size of the median filter.

        :return: The median filter size.
        """

        self.request.create_read(CommandId.MEDIAN_FILTER_SIZE.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_median_filter_size(self, size: int):
        """
        Set the size of the median filter.

        :param size: The median filter size.
        """

        if size < 3 or size > 32:
            raise ValueError("size must be from 3 to 32.")

        self.request.create_write_uint32(CommandId.MEDIAN_FILTER_SIZE.value, size)
        self.send_request_get_response()

    def get_smooth_filter_enable(self) -> bool:
        """
        Get the enabled state of the smooth filter.

        :return: The smooth filter state.
        """

        self.request.create_read(CommandId.SMOOTH_FILTER_ENABLE.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_smooth_filter_enable(self, enable: bool):
        """
        Set the enabled state of the smooth filter.

        :param enable: The smooth filter state.
        """

        self.request.create_write_uint8(CommandId.SMOOTH_FILTER_ENABLE.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_smooth_filter_factor(self) -> int:
        """
        Get the factor of the smooth filter.

        :return: The smooth filter factor.
        """

        self.request.create_read(CommandId.SMOOTH_FILTER_FACTOR.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_smooth_filter_factor(self, factor: int):
        """
        Set the factor of the smooth filter.

        :param factor: The smooth filter factor.
        """

        if factor < 1 or factor > 99:
            raise ValueError("factor must be from 1 to 99.")

        self.request.create_write_uint32(CommandId.SMOOTH_FILTER_FACTOR.value, factor)
        self.send_request_get_response()

    def get_baud_rate(self) -> BaudRate:
        """
        Get the baud rate used by the serial interface.

        :return: The baud rate.
        """

        self.request.create_read(CommandId.BAUD_RATE.value)
        self.send_request_get_response()
        return self.response.parse_uint8()

    def set_baud_rate(self, baud_rate: BaudRate):
        """
        Set the baud rate used by the serial interface. This parameter only takes effect when the serial interface is first enabled after power-up or restart.

        :param baud_rate: The baud rate.
        """

        self.request.create_write_uint8(CommandId.BAUD_RATE.value, baud_rate)
        self.send_request_get_response()

    def get_i2c_address(self) -> int:
        """
        Get the I2C address of the device.

        :return: The I2C address.
        """

        self.request.create_read(CommandId.I2C_ADDRESS.value)
        self.send_request_get_response()
        return self.response.parse_uint8()

    def set_i2c_address(self, address: int):
        """
        Set the I2C address of the device. This parameter only takes effect when the serial interface is first enabled after power-up or restart.

        :param address: The I2C address.
        """

        self.request.create_write_uint8(CommandId.I2C_ADDRESS.value, address)
        self.send_request_get_response()

    def get_rolling_average_enable(self) -> bool:
        """
        Get the enabled state of the rolling average filter.

        :return: The rolling average filter state.
        """

        self.request.create_read(CommandId.ROLLING_AVERAGE_ENABLE.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_rolling_average_enable(self, enable: bool):
        """
        Set the enabled state of the rolling average filter.

        :param enable: The rolling average filter state.
        """

        self.request.create_write_uint8(CommandId.ROLLING_AVERAGE_ENABLE.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_rolling_average_size(self) -> int:
        """
        Get the size of the rolling average filter.

        :return: The rolling average filter size.
        """

        self.request.create_read(CommandId.ROLLING_AVERAGE_SIZE.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def set_rolling_average_size(self, size: int):
        """
        Set the size of the rolling average filter.

        :param size: The rolling average filter size.
        """

        if size < 2 or size > 32:
            raise ValueError("size must be from 2 to 32.")

        self.request.create_write_uint32(CommandId.ROLLING_AVERAGE_SIZE.value, size)
        self.send_request_get_response()

    def set_sleep(self):
        """
        Puts the device into sleep mode. This mode is only available in serial UART communication mode. The device is then awakened by any activity on the Serial UART communication lines and will resume previous operation.
        """

        self.request.create_write_uint8(CommandId.SLEEP.value, 123)
        self.send_request_get_response()

    def get_led_state(self) -> bool:
        """
        Get the state of the LED.

        :return: The LED state.
        """

        self.request.create_read(CommandId.LED_STATE.value)
        self.send_request_get_response()
        return self.response.parse_uint8() != 0

    def set_led_state(self, enable: bool):
        """
        Set the state of the LED.

        :param enable: The LED state.
        """

        self.request.create_write_uint8(CommandId.LED_STATE.value, 1 if enable else 0)
        self.send_request_get_response()

    def get_zero_offset(self) -> int:
        """
        Get the zero offset. The zero offset is used to calibrate the distance sensor. The zero offset is added to the distance measurement.

        :return: The zero offset in cm.
        """

        self.request.create_read(CommandId.ZERO_OFFSET.value)
        self.send_request_get_response()
        return self.response.parse_int32()* 10

    def set_zero_offset(self, offset_cm: int):
        """
        Set the zero offset. The zero offset is used to calibrate the distance sensor. The zero offset is added to the distance measurement.

        :param offset_cm: The zero offset in cm.
        """

        self.request.create_write_int32(CommandId.ZERO_OFFSET.value, offset_cm/ 10)
        self.send_request_get_response()

    

    # -----------------------------------------------------------------------------
    # Helpers and composed requests.
    # -----------------------------------------------------------------------------\

    def initiate_serial(self):
        """
        When communicating with the GRF-500 over the serial interface, it is
        important to initiate serial mode. This is only required if the startup
        mode is 'Wait for interface'.
        """

        self.service_handler.write(b"UUU")


    def get_product_information(self):
        """
        Get the all the basic product information.

        :return: A named tuple containing the product name, hardware version, firmware version,
        and serial number.
        """

        return ProductInformation(
            self.get_product_name(),
            self.get_hardware_version(),
            self.get_firmware_version(),
            self.get_serial_number(),
        )


    def sleep(self):
        """
        Puts the device into sleep mode. This mode is only available in serial
        UART communication mode. The device is then awakened by any activity on the
        Serial UART communication lines and will resume previous operation.
        """

        self.set_sleep()


    def reset(self):
        """
        Restart the device as if it had been power cycled. You will need to
        re-establish the serial connection after this command.
        """

        self.set_reset(self.get_token())


    def save_parameters(self):
        """
        Save the current persistable device parameters.
        """

        self.set_save_parameters(self.get_token())


    # -----------------------------------------------------------------------------
    # Streaming helpers.
    # -----------------------------------------------------------------------------\
    def wait_for_streamed_distance_data(self, config: DistanceConfig, timeout: float = 1) -> DistanceDataCm | None:
        """
        Get the next streamed distance data.

        :param config: The distance configuration flags.
        :param timeout: The timeout in seconds, if 0 then this function is non-blocking.
        :return: Distance data or None if the timeout is reached.
        """

        if not self.wait_for_next_response(CommandId.DISTANCE_DATA.value, timeout):
            return None

        return DistanceDataCm(self.response, config)

    def wait_for_streamed_multi_data(self, timeout: float = 1) -> MultiData | None:
        """
        Get the next streamed multi signal distance data.

        :param timeout: The timeout in seconds, if 0 then this function is non-blocking.
        :return: Multi data or None if the timeout is reached.
        """

        if not self.wait_for_next_response(CommandId.MULTI_DATA.value, timeout):
            return None

        return MultiData(self.response)

