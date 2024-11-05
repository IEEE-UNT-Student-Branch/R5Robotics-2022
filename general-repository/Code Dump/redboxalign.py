import cv2
import numpy as np

cap = cv2.VideoCapture(0)

while True:
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Convert BGR to HSV
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Define the range of red color in HSV
    lower_red = np.array([0, 50, 50])
    upper_red = np.array([10, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red, upper_red)

    lower_red = np.array([170, 50, 50])
    upper_red = np.array([180, 255, 255])
    mask2 = cv2.inRange(hsv, lower_red, upper_red)

    # Combine the masks
    mask = cv2.bitwise_or(mask1, mask2)

    # Apply a Gaussian blur to the mask
    blur = cv2.GaussianBlur(mask, (9, 9), 0)

    # Find contours in the mask
    contours, _ = cv2.findContours(blur, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # Sort the contours by size
    contours = sorted(contours, key=cv2.contourArea, reverse=True)

    # Get the screen dimensions
    height, width, _ = frame.shape

    # Calculate the center of the screen
    center = int(width/2)

    # Check the largest contour
    if contours:
        contour = contours[0]
        M = cv2.moments(contour)
        if M["m00"] != 0:
            cx = int(M["m10"] / M["m00"])
            if cx < center-75:
                print("Red object is on the left")
            elif cx > center+75:
                print("Red object is on the right")
            else:
                print("Red object is in the center")

    # Display the resulting frame
    cv2.imshow('frame1', frame)
    cv2.imshow('frame2',mask)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything is done, release the capture
cap.release()
cv2.destroyAllWindows()
