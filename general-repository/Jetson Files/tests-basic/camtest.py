import cv2

cap = cv2.VideoCapture(0)

while True:
	_, frame = cap.read()
	cv2.imshow("cap", frame)
	if cv2.waitKey(5) & 0xFF == 27:
		break

cv2.destroyAllWindows()