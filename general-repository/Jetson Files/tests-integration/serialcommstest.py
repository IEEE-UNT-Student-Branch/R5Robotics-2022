import serial
from time import sleep

### SETUP
# - Jetson connected to Arduino, pins 8/10 (Jetson) to pins 19/18 (Arduino)
# - Arduino connected to laptop, for Serial Monitor debugging
# - Adjacent arduino sketch is called "serialcommstest.ino"
# - Monitor at baud 250000
# - Laptop communicates to Arduino through Serial (USB)
# - Arduino communicates to Jetson through Serial1 (pins 19/18)
# - Serial monitors have the ability to send messages; any message will be sent through Serial (USB)

# /dev/THS1 is UART_2 pins 8/10 (TX/RX)
# Note: For whatever reason, baud 9600, 250000, and 500000 works after ser.flush, but not 115200

BAUD_RATE = 250000
UART_PATH = '/dev/ttyTHS1'

with serial.Serial(UART_PATH, BAUD_RATE, timeout=10) as ser:
	ser.reset_input_buffer()
	ser.reset_output_buffer()

	while True:
		while ser.inWaiting():
			line = ser.readline()
			line = str(line)
			line = line.replace('b\'', '')
			line = line.replace('\'','')
			line = line.replace('\\n', '')
			line = line.replace('\\r', '')
			print("Serial found: " + line)
			sleep(.1)
			ser.write(bytes("Response: " + line,'utf-8'))
			ser.flush()
			sleep(.1)
