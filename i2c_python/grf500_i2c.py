"""
LightWare I2C API GRF-500
Version: 1.1.0
Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
https://www.lightwarelidar.com

This module provides a Python interface to the GRF-500 sensor over I2C interfaces.

License: MIT No Attribution (MIT-0)

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

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

from typing import NamedTuple, Protocol
from enum import Enum, IntFlag

class I2cServiceHandler(Protocol):
    """Provides an I2C port implementation to communicate with a physical device."""

    def __init__(self, bus_id: int, address: int = 0x66):
        ...

    def __del__(self):
        ...

    def read(self, register: int, length: int) -> bytearray:
        """Read a specified number of bytes from the I2C port."""
        ...

    def write(self, register: int, data: bytearray):
        """Write data to the I2C port."""
        ...

# -----------------------------------------------------------------------------
# Request.
# -----------------------------------------------------------------------------
class Request:
    """Handles the creation of request packets."""

    command_id: int | None
    data: bytearray
    write: bool
    length: int

    def __init__(self):
        self.command_id = None
        self.data = bytearray()
        self.write = False
        self.length = 0

    def build_read_packet(self, command_id: int, length: int):
        self.command_id = command_id
        self.data = bytearray()
        self.write = False
        self.length = length

    def build_write_packet(self, command_id: int, data: bytes):
        self.command_id = command_id
        self.data = bytearray(data)
        self.write = True
        self.length = len(data)

    def create_read_int8(self, command_id: int):
        self.build_read_packet(command_id, 1)

    def create_read_int16(self, command_id: int):
        self.build_read_packet(command_id, 2)

    def create_read_int32(self, command_id: int):
        self.build_read_packet(command_id, 4)

    def create_read_uint8(self, command_id: int):
        self.build_read_packet(command_id, 1)

    def create_read_uint16(self, command_id: int):
        self.build_read_packet(command_id, 2)
    
    def create_read_uint32(self, command_id: int):
        self.build_read_packet(command_id, 4)

    def create_read_data(self, command_id: int, size: int):
        self.build_read_packet(command_id, size)

    def create_read_data16(self, command_id: int):
        self.build_read_packet(command_id, 16)

    def create_write_int8(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 1, byteorder="little", signed=True))

    def create_write_int16(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 2, byteorder="little", signed=True))

    def create_write_int32(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 4, byteorder="little", signed=True))

    def create_write_uint8(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 1, byteorder="little", signed=False))

    def create_write_uint16(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 2, byteorder="little", signed=False))

    def create_write_uint32(self, command_id: int, value: int):
        self.build_write_packet(command_id, int.to_bytes(value, 4, byteorder="little", signed=False))

    def create_write_data(self, command_id: int, data: bytes):
        self.build_write_packet(command_id, data)

# -----------------------------------------------------------------------------
# Response.
# -----------------------------------------------------------------------------
class Response:
    """Handles the parsing of response packets."""

    command_id: int | None
    data: bytearray
    
    def __init__(self):
        self.reset()

    def reset(self):
        self.command_id = None
        self.data = bytearray()
    
    def parse_int8(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+1], byteorder="little", signed=True)

    def parse_int16(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+2], byteorder="little", signed=True)

    def parse_int32(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+4], byteorder="little", signed=True)

    def parse_uint8(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+1], byteorder="little", signed=False)

    def parse_uint16(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+2], byteorder="little", signed=False)

    def parse_uint32(self, offset: int = 0):
        return int.from_bytes(self.data[offset:offset+4], byteorder="little", signed=False)

    def parse_string(self, offset: int = 0):
        str16 = ""
        for i in range(0, 16):
            if self.data[offset + i] == 0:
                break
            else:
                str16 += chr(self.data[offset + i])

        return str16

    def parse_data(self, size: int, offset: int = 0):
        return self.data[offset : offset + size]


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

    def __init__(self, service_handler: I2cServiceHandler):
        self.service_handler = service_handler
        self.request = Request()
        self.response = Response()
        self.request_retries = 4
        self.request_timeout = 1.0

    # -----------------------------------------------------------------------------
    # Communication functions.
    # -----------------------------------------------------------------------------
    def send_request_get_response(self):
        if self.request.command_id is None:
            raise ValueError("Request command_id is not set.")

        if self.request.write:
            self.service_handler.write(self.request.command_id, self.request.data)
        else:
            self.response.data = self.service_handler.read(self.request.command_id, self.request.length)


    # -----------------------------------------------------------------------------
    # Device commands.
    # -----------------------------------------------------------------------------
    def get_product_name(self) -> str:
        """
        Get the product name.

        :return: The product name.
        """

        self.request.create_read_data16(CommandId.PRODUCT_NAME.value)
        self.send_request_get_response()
        return self.response.parse_string()

    def get_hardware_version(self) -> int:
        """
        Get the hardware version.

        :return: The hardware version.
        """

        self.request.create_read_uint32(CommandId.HARDWARE_VERSION.value)
        self.send_request_get_response()
        return self.response.parse_uint32()

    def get_firmware_version(self) -> str:
        """
        Get the firmware version.

        :return: The firmware version.
        """

        self.request.create_read_uint32(CommandId.FIRMWARE_VERSION.value)
        self.send_request_get_response()
        firmware_bytes = self.response.parse_data(3)
        firmware_str = f"{firmware_bytes[2]}.{firmware_bytes[1]}.{firmware_bytes[0]}"
        return firmware_str

    def get_serial_number(self) -> str:
        """
        Get the serial number.

        :return: The serial number.
        """

        self.request.create_read_data16(CommandId.SERIAL_NUMBER.value)
        self.send_request_get_response()
        return self.response.parse_string()

    def get_user_data(self) -> list[int]:
        """
        Get the user data.

        :return: 16-bytes of user data.
        """

        self.request.create_read_data16(CommandId.USER_DATA.value)
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

        self.request.create_read_uint16(CommandId.TOKEN.value)
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

        self.request.create_read_uint32(CommandId.DISTANCE_CONFIG.value)
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

        self.request.create_read_uint32(CommandId.STREAM.value)
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

        read_size = DistanceDataCm.get_size(config)
        self.request.create_read_data(CommandId.DISTANCE_DATA.value, read_size)
        self.send_request_get_response()
        return DistanceDataCm(self.response, config)

    def get_multi_data(self) -> MultiData:
        """
        Get 5 signals from the device. Each signal contains a distance and strength. If more than one target is identified within its field of view, the device will report up to five discrete target distances.

        :return: Multi data or None if the timeout is reached.
        """

        self.request.create_read_data(CommandId.MULTI_DATA.value, 44)
        self.send_request_get_response()
        return MultiData(self.response)

    def get_laser_firing(self) -> bool:
        """
        Get the laser firing state.

        :return: The laser firing state.
        """

        self.request.create_read_uint8(CommandId.LASER_FIRING.value)
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

        self.request.create_read_int32(CommandId.TEMPERATURE.value)
        self.send_request_get_response()
        return self.response.parse_int32()

    def get_auto_exposure(self) -> bool:
        """
        Get the auto exposure state.

        :return: The auto exposure state.
        """

        self.request.create_read_uint8(CommandId.AUTO_EXPOSURE.value)
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

        self.request.create_read_uint32(CommandId.UPDATE_RATE.value)
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

        self.request.create_read_data(CommandId.ALARM_STATUS.value, 2)
        self.send_request_get_response()
        return AlarmStatus(self.response)

    def get_alarm_return_mode(self) -> ReturnMode:
        """
        Get the return mode used for alarms.

        :return: The alarm return mode.
        """

        self.request.create_read_uint8(CommandId.ALARM_RETURN_MODE.value)
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

        self.request.create_read_uint32(CommandId.LOST_SIGNAL_COUNTER.value)
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

        self.request.create_read_uint32(CommandId.ALARM_A_DISTANCE.value)
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

        self.request.create_read_uint32(CommandId.ALARM_B_DISTANCE.value)
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

        self.request.create_read_uint32(CommandId.ALARM_HYSTERESIS.value)
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

        self.request.create_read_uint8(CommandId.GPIO_MODE.value)
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

        self.request.create_read_uint32(CommandId.GPIO_ALARM_CONFIRM_COUNT.value)
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

        self.request.create_read_uint8(CommandId.MEDIAN_FILTER_ENABLE.value)
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

        self.request.create_read_uint32(CommandId.MEDIAN_FILTER_SIZE.value)
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

        self.request.create_read_uint8(CommandId.SMOOTH_FILTER_ENABLE.value)
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

        self.request.create_read_uint32(CommandId.SMOOTH_FILTER_FACTOR.value)
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

        self.request.create_read_uint8(CommandId.BAUD_RATE.value)
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

        self.request.create_read_uint8(CommandId.I2C_ADDRESS.value)
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

        self.request.create_read_uint8(CommandId.ROLLING_AVERAGE_ENABLE.value)
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

        self.request.create_read_uint32(CommandId.ROLLING_AVERAGE_SIZE.value)
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

    def get_led_state(self) -> bool:
        """
        Get the state of the LED.

        :return: The LED state.
        """

        self.request.create_read_uint8(CommandId.LED_STATE.value)
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

        self.request.create_read_int32(CommandId.ZERO_OFFSET.value)
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

    def initiate_comms(self):
        """
        When communicating with the GRF-500 over the I2C interface, it is
        important to initiate I2C mode. This is only required if the startup
        mode is 'Wait for interface'.
        """

        self.service_handler.write(0, bytearray([0x80]))


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


    def reset(self):
        """
        Restart the device as if it had been power cycled. You will need to
        re-establish the communication connection after this command.
        """

        self.set_reset(self.get_token())


    def save_parameters(self):
        """
        Save the current persistable device parameters.
        """

        self.set_save_parameters(self.get_token())
