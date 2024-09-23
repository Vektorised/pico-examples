import serial
import struct
import time

# Set up the serial connection (replace '/dev/ttyACM0' with your port)
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)

# Wait briefly to establish connection
time.sleep(0.1)

control_word = b'I'  # Replace this with the actual byte for your command
ser.write(control_word)

print("Waiting for data...")
# Wait for the command to be processed
time.sleep(1)

control_word = b'E'  # Replace this with the actual byte for your command
ser.write(control_word)

# Read 8 bytes (2 * 4-byte integers)
data = ser.read(8)

# Check if we received the expected amount of data
if len(data) == 8:
    # Unpack the 8 bytes as two little-endian 32-bit integers
    encoder1_position, encoder2_position = struct.unpack('<ii', data)

    # Print the encoder positions
    print(f"Encoder 1: {encoder1_position}, Encoder 2: {encoder2_position}")
else:
    print("Incomplete data received")

# Close the serial port
ser.close()
