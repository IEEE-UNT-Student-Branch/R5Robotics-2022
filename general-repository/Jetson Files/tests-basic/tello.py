from djitellopy import Tello
import cv2
from time import sleep

print("starting")
bot = Tello()
bot.connect()
print("conencted")
bot.streamoff()
bot.streamon()

sleep(3)
#bot.takeoff()
print("takeoff")

while True:
	img = bot.get_frame_read().frame
	cv2.imshow("cam", img)
	if cv2.waitKey(2) & 0xFF == 27:
		break

sleep(3)
#bot.land()
print("land")

cv2.destroyAllWindows()
bot.streamoff()
bot.end()
print("end")

