"""
LightWare GRF-500 Python serial API example.
Version: 1.1.0
Copyright (c) 2025 LightWare Optoelectronics (Pty) Ltd.
https://www.lightwarelidar.com

This example demonstrates how to communicate with the GRF-500 over the serial interface.

Notes:
    Requires the pySerial module.

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

import time
import grf500_serial

# --------------------------------------------------------------------------------------------------
# Main application.
# --------------------------------------------------------------------------------------------------
print("Running GRF-500 sample.")

grf500 = grf500_serial.Grf500(grf500_serial.SerialServiceHandler("COM4", 115200))

# NOTE: Only needed if running over serial interface and mode is set to 'Wait for interface'.
grf500.initiate_serial()

product_info = grf500.get_product_information()

print("Product: " + product_info.product_name)
print("Hardware: " + str(product_info.hardware_version))
print("Firmware: " + product_info.firmware_version)
print("Serial: " + product_info.serial_number)


# Setup device
grf500.set_stream(grf500_serial.StreamId.NONE)
grf500.set_update_rate(5)
distance_config = grf500_serial.DistanceConfig.ALL
grf500.set_distance_config(distance_config)

# Poll for distance data
distance_data = grf500.get_distance_data(distance_config)
print("Polled distance: " + str(distance_data.first_return_raw_cm / 100.0) + " m")

# Stream distance data: Blocking version.
grf500.set_stream(grf500_serial.StreamId.DISTANCE)

for i in range(10):
    distance_data = grf500.wait_for_streamed_distance(distance_config, 1)

    if distance_data is not None:
        print("Streamed distance: " + str(distance_data.first_return_raw_cm / 100.0) + " m")
    else:
        print("Stream timeout")

# Stream distance data: Non-blocking version.
for i in range(10):
    print("Attempting to get response...")

    while True:
        # NOTE: The timeout is set to 0.
        distance_data = grf500.wait_for_streamed_distance(distance_config, 0)

        if distance_data is None:
            print("Full response not received yet, waiting/doing other work...")
            # NOTE: Simulate doing some work...
            time.sleep(0.05)
        else:
            print(
                "Non-blocking streamed distance: "
                + str(distance_data.first_return_raw_cm / 100.0)
                + " m"
            )
            break
