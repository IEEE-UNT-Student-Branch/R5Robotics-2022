import serial
from time import sleep
from random import randint

### SETUP
# - Jetson connected to Arduino, pins 8/10 (Jetson) to pins 19/18 (Arduino)
# - Arduino connected to laptop, for Serial Monitor debugging
# - Adjacent arduino sketch is called "jetsonarduinocomms.ino"

# /dev/THS1 is UART_2 pins 8/10 (TX/RX)

BAUD_RATE = 250000
UART_PATH = '/dev/ttyTHS1'
SERIAL_COMPLETE = False
ESTABLISH_CONN = False
GOTO_SUCCESS = True

# For encoding/decoding messages to/from Arduino
def str2byte(string: str):
	return bytes(string, 'utf-8')

def byte2str(bytestr):
	bytestr = str(bytestr)
	bytestr = bytestr.replace('b\'', '')
	bytestr = bytestr.replace('\'','')
	bytestr = bytestr.replace('\\n', '')
	bytestr = bytestr.replace('\\r', '')
	return bytestr

# Open/Reopen Serial
while not SERIAL_COMPLETE:
	with serial.Serial(UART_PATH, BAUD_RATE, timeout=10) as ser:

		# Flush buffers until connection established x3, otherwise reopen the serial port
		for i in range(3):
			ser.reset_input_buffer()
			ser.reset_output_buffer()

			print("Establishing connection...")
			ser.write(str2byte("establish"))
			ser.flush()
			sleep(1)
			if ser.inWaiting():
				line = ser.readline()
				line = byte2str(line)
				sleep(.1)
				if line == "establish success":
					ESTABLISH_CONN = True
					break
			else:
				print("Waiting for connection...")
			sleep(.1)

		# Send debug info
		if ESTABLISH_CONN:
			print("Connection Established, beginning program")
			sleep(1)
		else:
			print("Reinitializing Serial Port")
			sleep(1)

		# Analyze incoming data
		while ESTABLISH_CONN:
			if GOTO_SUCCESS:
				cmd = "goto, " + str(randint(0,1000)) + ", " + str(randint(0, 1000))
				print("Command: " + cmd)
				ser.write(str2byte(cmd))
				GOTO_SUCCESS = False
				sleep(.1)

			while ser.inWaiting():
				line = ser.readline()
				line = byte2str(line)
				print("Serial found: " + line)

				if line == "goto success":
					print("Relocation successful. Generating new position...")
					GOTO_SUCCESS = True
					sleep(1)
