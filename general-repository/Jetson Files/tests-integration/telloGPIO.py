from djitellopy import Tello
import cv2
from time import sleep
from os import chdir, system

# GPIO Setup
#TELLO_CONNECT_SIG = 22		##BCM 22 is physically pin 15 on the Nano
				#3.3V and 220 ohm resistor
try:
	#GPIO.setmode(GPIO.BCM)
	#GPIO.setup(TELLO_CONNECT_SIG, GPIO.OUT, initial=GPIO.LOW)
	system("echo 194 > /sys/class/gpio/export")
	system("echo out > /sys/class/gpio/gpio194/direction")
except:
	print("Failed to send GPIO signal")

print("Starting Tello")
bot = Tello()
bot.connect()
print("Tello Connected")
#GPIO.output(TELLO_CONNECT_SIG, GPIO.HIGH)
system("echo 1 > /sys/class/gpio/gpio194/value")
bot.streamoff()
bot.streamon()

sleep(3)
#bot.takeoff()
print("Tello Takeoff (disabled)")

while True:
	img = bot.get_frame_read().frame
	cv2.imshow("cam", img)
	if cv2.waitKey(2) & 0xFF == 27:
		break

sleep(3)
#bot.land()
print("Tello Land")

#GPIO.output(TELLO_CONNECT_SIG, GPIO.LOW)
system("echo 0 > /sys/class/gpio/gpio194/value")
cv2.destroyAllWindows()
bot.streamoff()
bot.end()
print("Tello Instance Ended")
exit()

