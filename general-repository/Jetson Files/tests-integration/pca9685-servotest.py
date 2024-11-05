from board import SCL, SDA
import busio
from adafruit_pca9685 import PCA9685
from time import sleep
from adafruit_motor import servo

i2c = busio.I2C(SCL, SDA)
pca = PCA9685(i2c)
pca.frequency = 50

servo1 = servo.Servo(pca.channels[0])
servo1.set_pulse_width_range(500, 2500) 	# By default, 750-2250 for safety, but only allowed 135 deg range
						# Always set 500-2500 for each motor to allow 180 deg range
print("running")
try:
	for i in range(0,182,2):
		servo1.angle = i
		print(i)
		sleep(.1)
finally:
	print("ending")
	servo1.angle = 0


