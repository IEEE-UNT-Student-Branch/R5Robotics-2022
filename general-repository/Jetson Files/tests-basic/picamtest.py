from CSICamera import *

# Set up Cameras
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


# Video Capture
if cam.video_capture.isOpened():
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        try:
            while True:
                _, img = cam.read()
                if cv2.getWindowProperty(window_name, cv2.WND_PROP_AUTOSIZE) >= 0:
                    cv2.imshow(window_name, img)
                else:
                    print("Error: No camera feed to show")
                    break

                if cv2.waitKey(5) & 0xFF == 27:
                    break
        finally:
            cam.stop()
            cam.release()
            cv2.destroyAllWindows()
else:
        print("Error: Unable to open both cameras")
        cam.stop()
        cam.release()
