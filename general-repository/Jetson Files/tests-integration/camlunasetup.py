'''
 Setup: PCA9685 board connected to the Jetson at ports 3/5 (SDA/SCL)
   Camera Left/Right Servo attached to PCA channel port 0
   Camera Up/Down Servo port 1
   Right Luna Servo port 2
   Left Luna Servo port 3

   Right Luna Sensor connected to Jetson at ports 3/5 (SDA/SCL)
   Left Luna ports 3/5
   (Assumed Right Luna is address 0x11)
'''

# Port Constants
LR_SRVO_CHNL = 0
UD_SRVO_CHNL = 1
RLUNA_SRVO_CHNL = 2
LLUNA_SRVO_CHNL = 3
RLUNA_ADDR = 0x11
LLUNA_ADDR = 0x10
LUNA_I2C_PORT = 1	#SDA/SCL pins 27/28 are I2C port 0. Pins 3/5 are I2C port 1

# Camera Servo Mvmt Constants
UD_FWD = 0
UD_UP = 90
LR_LEFT = 180
LR_FWD = 90
LR_RIGHT = 0

# Luna Servo Mvmt Constants
LUNA_OFFSET = 30	#This will be the angle the Luna is at when pointed inwards
LLUNA_IN = 0
LLUNA_FWD = 0 + LUNA_OFFSET
LLUNA_LEFT = 90 + LUNA_OFFSET
LLUNA_BACK = 180
RLUNA_IN = 180
RLUNA_FWD = 180 - LUNA_OFFSET
RLUNA_RIGHT = 90 - LUNA_OFFSET
RLUNA_BACK = 0

# Enable/Disable certain functionalities (testing)
ENABLE_CAMERA = False
ENABLE_PCA_SERVOS = True
ENABLE_LUNAS = True


from board import SCL, SDA
import busio
from adafruit_pca9685 import PCA9685
from time import sleep
from adafruit_motor import servo
from tfli2c import TFLuna
from CSICamera import *
#from threading import Thread

# Pi Camera Capture/OpenCV connection
if ENABLE_CAMERA:
	try:
		window_name = "Pi Cam"
		cam = CSI_Camera()
		cam.open(
		gstreamer_pipeline(
		    sensor_id=0,
		    capture_width=1920,
		    capture_height=1080,
		    flip_method=0,
		    display_width=960,
		    display_height=540,
		)
		)
		cam.start()
	except Exception as e:
		raise Exception("Camera startup fucked up")
		raise Exception("Error: " + str(e))
		exit()

# Servo Port Settings
if ENABLE_PCA_SERVOS:
	try:
		print("Kevin Note: This next process will take at most 5-10 seconds")
		i2c = busio.I2C(SCL, SDA)
		pca = PCA9685(i2c)
		pca.frequency = 50
	except Exception as e:
		raise Exception("PCA fucked up")
		raise Exception("Error: " + str(e))
		exit()
	try:
		srvLR = servo.Servo(pca.channels[LR_SRVO_CHNL])
		srvLR.set_pulse_width_range(500, 2500)
		srvUD = servo.Servo(pca.channels[UD_SRVO_CHNL])
		srvUD.set_pulse_width_range(500, 2500)
		srvLunaL = servo.Servo(pca.channels[LLUNA_SRVO_CHNL])
		srvLunaL.set_pulse_width_range(500, 2500)
		srvLunaR = servo.Servo(pca.channels[RLUNA_SRVO_CHNL])
		srvLunaR.set_pulse_width_range(500, 2500)
	except Exception as e:
		raise Exception("Servo Channel instantiation fucked up")
		raise Exception("Error: " + str(e))
		exit()

# Luna Connection
if ENABLE_LUNAS:
	try:
		TFLunaL = TFLuna(LLUNA_ADDR, LUNA_I2C_PORT)
		TFLunaL.getData()
		TFLunaR = TFLuna(RLUNA_ADDR, LUNA_I2C_PORT)
		TFLunaR.getData()
	except Exception as e:
		raise Exception("TFLuna fucked up")
		raise Exception("Error: " + str(e))
		exit()

'''
 Function Definitions
'''
#  Simultaneously set the two camera motors
#  - @lr : Left/Right servo angle
#  - @ud : Up/Down servo angle
def setCameraMotors(lr: int, ud: int):
	srvLR.angle = lr
	srvUD.angle = ud

#  Set a single motor to a specific angle
#  - @serv : Servo Object to set
#  - @ang : Desired angle
def setMotor(serv, ang: int):
	serv.angle = ang

def updateLunas():
	TFLunaL.getData()
	TFLunaR.getData()

#  Sequence that will calculate the ground unit's offset from the center of the box's opening
#   * assumes that the ground unit is directly facing the box's opening head on (not at an angle)
#   * assumes that there is 75 cm of distance from the opening to the ground unit
#   * assumes that positive is right and negative is left
#  - @return : An integer in cm of the offset (can be +/-)
def getOffsetBoxOpening() -> int:
	LLuna_data = []
	RLuna_data = []
	setMotor(srvLunaL, LLUNA_IN)
	setMotor(srvLunaR, RLUNA_IN)
	sleep(.75)

	for i in range(0, 181, 2):
		setMotor(srvLunaL, i)
		setMotor(srvLunaR, 180 - i)
		sleep(0.01)
		updateLunas()
		#if TFLunaL.dist < 75:
		#	pass
		#else:
		#	pass #set motor
		#if TFLunaR.dist < 75:
		#	pass
		#else:
		#	pass #set motor
		LLuna_data.append((i - LUNA_OFFSET, TFLunaL.dist))
		RLuna_data.append((i - LUNA_OFFSET, TFLunaR.dist))
		print("L: " + str(TFLunaL.dist) + ", R: " + str(TFLunaR.dist))
		sleep(0.01)

	#LLuna_sort = sorted(LLuna_data, key=lambda x: x[1])
	#RLuna_data = sorted(RLuna_data, key=lambda x: x[1])

	setMotor(srvLunaL, LLUNA_FWD)
	setMotor(srvLunaR, RLUNA_FWD)

	sleep(.75)

	print("L_Data: " + str(LLuna_data))
	print("R_Data: " + str(RLuna_data))

	return -1

'''
 Begin
    Reference:
     [Servo Objects]
      srvLR
      srvUD
      srvLunaL
      srvLunaR

     [Luna Objects]
      TFLunaL
      TFLunaR

     [Usable functions]
      setMotor(ServoObject, angle)			# Set a single, specific motor to a specific angle
      setCameraMotors(LRServoAngle, UDServoAngle)	# Set both camera servo angles in one function
      updateLunas()
'''
if __name__ == "__main__": 
	try:
		# Main process (begin)
		print("Process start")
		setMotor(srvLR, LR_RIGHT)
		setMotor(srvUD, UD_UP)

		getOffsetBoxOpening()
		# Main process (loop)
		#while True:
		#	print("L: " + str(TFLunaL.dist) + ", R: " + str(TFLunaR.dist))
		#	updateLunas()
		#	sleep(0.1)

	except Exception as e:
		print("Process fucked up")
		print(e)

	finally:
		setCameraMotors(LR_FWD, UD_FWD)
		setMotor(srvLunaL, LLUNA_FWD)
		setMotor(srvLunaR, RLUNA_FWD)
		sleep(1)
		print("Process finished")
		exit()
