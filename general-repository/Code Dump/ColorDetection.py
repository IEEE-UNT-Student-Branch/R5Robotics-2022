import cv2
import numpy as np

# Constants
blueRangeUpperHSV = (122, 255, 255)     # Blue
blueRangeLowerHSV = (78, 121, 72)

redRangeUpperHSV = (24, 255, 255)       # Red
redRangeLowerHSV = (0, 163, 0)

brownRangeUpperHSV = (17, 95, 255)      # Brown (Broken)
brownRangeLowerHSV = (0, 20, 0)


# Function Definitions
def empty():
    pass

def getContours(img):
    # Contour Collection
    contours, heirarchy = cv2.findContours(img,
                                           cv2.RETR_EXTERNAL,       # Retrieve the extreme outer contours
                                           cv2.CHAIN_APPROX_NONE)   # Request ALL contour values without compressing

    # Contour Processing
    for cnt in contours:
        area = cv2.contourArea(cnt)  # For possible later use

        # Draw Contours
        if area > 700:
            # Draw ALL contours
            cv2.drawContours(frameContours, cnt, -1, (0, 255, 0), 3)  # (image, contour, contourIndex, color, thickness)
                                                                      # contourIndex = -1 means draw all contour points

            # Find Perimeter and Corners
            cntperim = cv2.arcLength(cnt, True)  # Calculate contour perimeter (True expecting closed shapes)
            approxcorners = cv2.approxPolyDP(cnt, 0.02 * cntperim, True)  # Approximate the best fit shape for the contour

            # Bounding boxes for analysis
            x, y, w, h = cv2.boundingRect(approxcorners)                            # Standard Bounding box (Blue)
            cv2.rectangle(frameContours, (x, y), (x + w, y + h), (255, 0, 0), 2)
            rect = cv2.minAreaRect(cnt)                                             # Rotatable Bounding box (Red)
            box = cv2.boxPoints(rect)
            box = np.int0(box)
            cv2.drawContours(frameContours, [box], 0, (0, 0, 255), 2)


# VideoCapture settings
CAMERA_ONE = 0
CAMERA_TWO = 1
cap = cv2.VideoCapture(CAMERA_TWO)
cap.set(10, 100)
cap.set(11, 100)

# Main loop
while True:

    # Source frames
    _, frame = cap.read()
    frameHSV = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # HSV Masks
    lowerRangeBlue = np.array(blueRangeLowerHSV)
    upperRangeBlue = np.array(blueRangeUpperHSV)
    lowerRangeRed = np.array(redRangeLowerHSV)
    upperRangeRed = np.array(redRangeUpperHSV)
    lowerRangeBrown = np.array(brownRangeLowerHSV)
    upperRangeBrown = np.array(brownRangeLowerHSV)

    frameMaskBlue = cv2.inRange(frameHSV, lowerRangeBlue, upperRangeBlue)
    frameMaskRed = cv2.inRange(frameHSV, lowerRangeRed, upperRangeRed)
    frameMaskBrown = cv2.inRange(frameHSV, lowerRangeBrown, upperRangeBrown)

    frameMask = frameMaskRed | frameMaskBlue | frameMaskBrown

    maskResult = cv2.bitwise_and(frame, frame, mask=frameMask)
    frameContours = maskResult.copy()

    # Frame processing
    frameGrey = cv2.cvtColor(maskResult, cv2.COLOR_BGR2GRAY)
    frameBlur = cv2.GaussianBlur(frameGrey, (5, 5), 1)
    frameCanny = cv2.Canny(frameBlur, 75, 200, 1)

    cv2.imshow('Frame', frame)
    # cv2.imshow('Frame Mask', frameMask)
    # cv2.imshow('Mask Result', maskResult)

    getContours(frameCanny)
    cv2.imshow('Contour Detection', frameContours)


    # # setting values for base colors
    # b = res[:, :, :1]
    # g = res[:, :, 1:2]
    # r = res[:, :, 2:]
    #
    # # computing the mean
    # b_mean = np.mean(b)
    # g_mean = np.mean(g)
    # r_mean = np.mean(r)
    #
    # # displaying if red
    # if ( r_mean > 1):
    #     print("red")

    if cv2.waitKey(1) & 0xFF == 27:
        break