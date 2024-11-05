import RPi.GPIO as GPIO
from time import sleep
from os import system, chdir

BUTTON_SIG = 23		#BCM 23 is physically pin 16 on the Nano
			#3.3V and 1k ohm resistor
POW_LED_SIG = 18	#BCM 18 is physically pin 12
			#3.3V and 220 ohm resistor
toggle_LED = False

GPIO.setmode(GPIO.BCM)
GPIO.setup(BUTTON_SIG, GPIO.IN)
GPIO.setup(POW_LED_SIG, GPIO.OUT, initial=GPIO.LOW)

def LED_SWITCH():
	global toggle_LED
	toggle_LED = not toggle_LED
	print("Toggle: " + str(toggle_LED))

try:
	while True:
		if not GPIO.input(BUTTON_SIG):
			GPIO.output(POW_LED_SIG, GPIO.HIGH)
			print("Running/Starting docker")
			chdir('/home/kevin')
			system('./dockerboot.sh')
			# Freeze on this line until docker is finished with its processes
			GPIO.output(POW_LED_SIG, GPIO.LOW)
			break
finally:
	print("cleaning")
	GPIO.cleanup()
