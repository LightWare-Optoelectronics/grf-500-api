"""
LightWare GRF-500 Python I2C API example.
Version: 1.1.0
Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
https://www.lightwarelidar.com

This example demonstrates how to communicate with the GRF-500 over the I2C interface.

Notes:
    Requires the smbus2 module.

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

import time
import grf500_i2c

# -----------------------------------------------------------------------------
# I2C service handler implementation.
# -----------------------------------------------------------------------------
# NOTE: You must provide an appropriate implementation for your platform.

# This implementation uses smbus which is common on Linux based platforms.
import smbus2

class BasicI2cHandler():
    """Provides an I2C port implementation to communicate with a physical device."""

    port: smbus2.SMBus
    address: int

    def __init__(self, bus_id: int, address: int = 0x66):
        self.port = smbus2.SMBus(bus_id)
        self.address = address
        
    def __del__(self):
        self.port.close()

    def read(self, register: int, length: int) -> bytearray:
        """Read a specified number of bytes from the I2C port."""

        return self.port.read_i2c_block_data(self.address, register, length)

    def write(self, register: int, data: bytearray):
        """Write data to the I2C port."""

        self.port.write_i2c_block_data(self.address, register, data)

# --------------------------------------------------------------------------------------------------
# Main application.
# --------------------------------------------------------------------------------------------------
print("Running GRF-500 sample.")

i2c_address = 0x66
i2c_bus_id = 1

grf500 = grf500_i2c.Grf500(BasicI2cHandler(i2c_bus_id, i2c_address))

# NOTE: Only needed if running over I2C interface and mode is set to 'Wait for interface'.
grf500.initiate_comms()

# --------------------------------------------------------------------------------------------------
# Get production information.
# --------------------------------------------------------------------------------------------------
product_info = grf500.get_product_information()

print("Product: " + product_info.product_name)
print("Hardware: " + str(product_info.hardware_version))
print("Firmware: " + product_info.firmware_version)
print("Serial: " + product_info.serial_number)

# --------------------------------------------------------------------------------------------------
# Basic setup.
# --------------------------------------------------------------------------------------------------
# Set update rate to 5 Hz.
grf500.set_update_rate(5)

# --------------------------------------------------------------------------------------------------
# Example of getting extensive distance data.
# --------------------------------------------------------------------------------------------------
print("Getting extensive distance data:")

# Set distance configuration to output all distance data types.
distance_config = grf500_i2c.DistanceConfig.ALL
grf500.set_distance_config(distance_config)

print("First return | last return | temperature")

# Poll for 10 distance data readings.
for i in range(10):
    distance_data = grf500.get_distance_data(distance_config)
    print(str(distance_data.first_return_raw_cm / 100.0) + " m (" + str(distance_data.first_return_strength) + ") | " +
          str(distance_data.last_return_raw_cm / 100.0) + " m (" + str(distance_data.last_return_strength) + ") | " +
          str(distance_data.temperature / 100.0) + " C")

    # Wait for next update (based on update rate).
    time.sleep(1.0 / 5.0)

# --------------------------------------------------------------------------------------------------
# Example of getting last return distance data.
# --------------------------------------------------------------------------------------------------
print("Getting last return distance data:")

# Set distance configuration to just get the last return.
distance_config = grf500_i2c.DistanceConfig.LAST_RETURN_RAW
grf500.set_distance_config(distance_config)

# Poll for 10 distance data readings.
while True:
    distance_data = grf500.get_distance_data(distance_config)
    print("Distance: " + str(distance_data.last_return_raw_cm / 100.0) + " m")

    # Wait for next update (based on update rate).
    time.sleep(1.0 / 5.0)
