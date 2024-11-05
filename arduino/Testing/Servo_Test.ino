#include <Servo.h>
#include <TFLI2C.h>

TFLI2C tflI2C;
// Use these defraults or insert your own values
int16_t  tfDist;    // distance in centimeters
int16_t  tfAddrfront = TFL_DEF_ADR;  // Use this default I2C address 0
int16_t  tfAddrleft = 0x11;  // Use this default I2C address
int walldis = 70;

// Create Servo objects
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// Define constants for servo positions
const int SERVO_MIN_ANGLE = 0;
const int SERVO_MAX_ANGLE = 180;
const int SERVO_STEP_ANGLE = 1; // adjust to change servo speed

// Define variables for servo positions
int servo1_position = SERVO_MIN_ANGLE;
int servo2_position = SERVO_MIN_ANGLE;
int servo3_position = SERVO_MIN_ANGLE;
int servo4_position = SERVO_MIN_ANGLE;

// Define variables to track servo direction
int servo1_direction = 1;
int servo2_direction = 1;
int servo3_direction = 1;
int servo4_direction = 1;

void setup() {
  // Attach servos to their pins
  servo1.attach(3);
  servo2.attach(4);
  servo3.attach(5); // this is the tilt servo in the pan/tilt combo for pycam
  servo4.attach(6);
}

void loop() {
  // Update servo positions
  if (servo1_position >= SERVO_MIN_ANGLE && servo1_position <= SERVO_MAX_ANGLE) {
    servo1.write(servo1_position);
    servo1_position += servo1_direction * SERVO_STEP_ANGLE;
    delay(10); // Wait a short period of time
  }
  if (servo1_position >= SERVO_MAX_ANGLE) {
    servo1_direction = -1;
  }
  if (servo1_position <= SERVO_MIN_ANGLE) {
    servo1_direction = 1;
  }
  
  if (servo2_position >= SERVO_MIN_ANGLE && servo2_position <= SERVO_MAX_ANGLE) {
    servo2.write(servo2_position);
    servo2_position += servo2_direction * SERVO_STEP_ANGLE;
    delay(10); // Wait a short period of time
  }
  if (servo2_position >= SERVO_MAX_ANGLE) {
    servo2_direction = -1;
  }
  if (servo2_position <= SERVO_MIN_ANGLE) {
    servo2_direction = 1;
  }
  
  // if (servo3_position >= SERVO_MIN_ANGLE && servo3_position <= SERVO_MAX_ANGLE) {
  //   servo3.write(servo3_position);
  //   servo3_position += servo3_direction * SERVO_STEP_ANGLE;
  //   delay(15); // Wait a short period of time
  // }
  // if (servo3_position >= SERVO_MAX_ANGLE) {
  //   servo3_direction = -1;
  // }
  // if (servo3_position <= SERVO_MIN_ANGLE) {
  //   servo3_direction = 1;
  // }
  
  if (servo4_position >= SERVO_MIN_ANGLE && servo4_position <= SERVO_MAX_ANGLE) {
    servo4.write(servo4_position);
    servo4_position += servo4_direction * SERVO_STEP_ANGLE;
    delay(10); // Wait a short period of time
  }
  if (servo4_position >= SERVO_MAX_ANGLE) {
    servo4_direction = -1;
  }
  if (servo4_position <= SERVO_MIN_ANGLE) {
    servo4_direction = 1;
  }
}
