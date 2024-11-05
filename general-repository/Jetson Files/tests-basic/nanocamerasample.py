import nanocamera as nanocam
import cv2

# Note, use device_id=# to choose from multiple CSI cameras
camera = nanocam.Camera(device_id=0, flip=0, width=1280, height=800, fps=0)
print('Camera Ready - ', camera.isReady())

try:
	while camera.isReady():
		try:
			frame = camera.read()
			cv2.imshow("NanoCamera", frame)
			if cv2.waitKey(5) & 0xFF == 27:
				break
		except KeyboardInterrupt:
			break
except Exception as e:
	print("Fuck up: " + str(e))
finally:
	print(camera.hasError())
	camera.release()
	del camera
