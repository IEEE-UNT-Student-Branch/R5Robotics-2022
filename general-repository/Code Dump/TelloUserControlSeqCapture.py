"""
# DJI Tello User Control
#  WASD - Lateral movement
#  QE - Yaw movement
#  Up/Down Arrows - Elevation
#  C - Camera Capture at 1 second intervals
"""
from sys import exit
from djitellopy import Tello
import KeyPressModule as kp
import cv2

# Tello startup sequence
print("Connecting to Tello...")
telloBot = Tello()
telloBot.connect()
telloBot.for_back_velocity = 0
telloBot.left_right_velocity = 0
telloBot.up_down_velocity = 0
telloBot.yaw_velocity = 0
telloBot.speed = 0
print("Tello Connected.")
print("Battery life: " + str(telloBot.get_battery()) + "%")
telloBot.streamoff()  # Camera turn off and turn on
telloBot.streamon()
kp.init()

'''
 User input to control drone
'''
LINEAR_SPEED = 15
ANGULAR_SPEED = 50
def userinput(drone: Tello):
    leftright, forwardback, updown, yaw = 0, 0, 0, 0

    # Drone Control Decision Tree
    if kp.getKey("w"):  # Forward/Backward
        forwardback = LINEAR_SPEED
    elif kp.getKey("s"):
        forwardback = -LINEAR_SPEED

    if kp.getKey("a"):  # Left/Right (Lateral)
        leftright = -LINEAR_SPEED
    elif kp.getKey("d"):
        leftright = LINEAR_SPEED

    if kp.getKey("q"):  # Left/Right (Yaw)
        yaw = -ANGULAR_SPEED
    elif kp.getKey("e"):
        yaw = ANGULAR_SPEED

    if kp.getKey("r"):  # Up/Down
        updown = LINEAR_SPEED
    elif kp.getKey("f"):
        updown = -LINEAR_SPEED

    if not drone.is_flying and kp.getKey("t"):  # Liftoff/Landing
        drone.takeoff()
    elif drone.is_flying and kp.getKey("g"):
        drone.land()

    # Send directional commands to the drone
    # drone.send_rc_control(leftright, forwardback, updown, yaw)
    cmd = 'rc {} {} {} {}'.format(leftright, forwardback, updown, yaw)
    drone.send_command_without_return(cmd)


'''
 Tello Main Loop
'''
cv2.waitKey(300)
while True:
    # Onboard Camera Display
    img = telloBot.get_frame_read().frame
    cv2.imshow("Bot Camera", img)

    # Input Control
    userinput(telloBot)

    # Exit Program conditions
    if cv2.waitKey(2) & 0xFF == 27:
        break

cv2.destroyAllWindows()
telloBot.streamoff()
telloBot.end()
kp.quit()
print("Ending Program")
exit()
