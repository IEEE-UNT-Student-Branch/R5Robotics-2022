import cv2
import numpy as np

# Trackbar Predefinitions
def empty():
    pass

cv2.namedWindow("TrackBars")
cv2.resizeWindow("TrackBars", 640, 240)
cv2.createTrackbar("Hue Min", "TrackBars", 0, 179, empty)  # (name, windowname, initvalue, maxvalue, function)
cv2.createTrackbar("Hue Max", "TrackBars", 179, 179, empty)
cv2.createTrackbar("Sat Min", "TrackBars", 0, 255, empty)
cv2.createTrackbar("Sat Max", "TrackBars", 255, 255, empty)
cv2.createTrackbar("Val Min", "TrackBars", 0, 255, empty)
cv2.createTrackbar("Val Max", "TrackBars", 255, 255, empty)

# Video Capture settings
cap = cv2.VideoCapture(0)
cap.set(10, 100)  # Set brightness 100
cap.set(11, 100)  # Set contrast 100

# Main loop
while True:

    # Source frames
    _, frame = cap.read()
    frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Trackbar get values
    hue_min = cv2.getTrackbarPos("Hue Min", "TrackBars")
    hue_max = cv2.getTrackbarPos("Hue Max", "TrackBars")
    sat_min = cv2.getTrackbarPos("Sat Min", "TrackBars")
    sat_max = cv2.getTrackbarPos("Sat Max", "TrackBars")
    val_min = cv2.getTrackbarPos("Val Min", "TrackBars")
    val_max = cv2.getTrackbarPos("Val Max", "TrackBars")
    lower_range = np.array((hue_min, sat_min, val_min))
    upper_range = np.array((hue_max, sat_max, val_max))
    frameMask = cv2.inRange(frameHSV, lower_range, upper_range)
    maskResult = cv2.bitwise_and(frame, frame, mask=frameMask)

    # Show frames
    cv2.imshow('frame', frame)
    cv2.imshow('mask', frameMask)
    cv2.imshow('res', maskResult)

    if cv2.waitKey(1) & 0xFF == 27:
        print("Final HSV values: ")
        print("Hue Max-Min: " + str(hue_max) + " - " + str(hue_min))
        print("Sat Max-Min: " + str(sat_max) + " - " + str(sat_min))
        print("Val Max-Min: " + str(val_max) + " - " + str(val_min))
        break
