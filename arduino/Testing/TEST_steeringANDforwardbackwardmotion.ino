#include <Servo.h>

Servo motor;            //Create ESC drive motor 'motor' and steering servo 'steer'
Servo steer;

int speedRead;          //Vars necessary to control the speed of the motor via the ESC.
int speedRemap;
int motorSpeed;
int forwardSpeed=1545;
int turnSpeed=1550;
int brakeSpeed=1001;
int reverseSpeed=1470;
int speedCenter = 1500;
int reverse_time;

const int neutral = 90;           //Steering servo positions for straight, left turns, and right turns. These can be changed and are set in degrees.
const int steerRight = 55;
const int steerLeft = 125;

void setup() {
  Serial.begin(115200);             //Begins //Serial Monitor for debugging
  motor.attach(2, 1000, 2000);           //Attaches motor to pin 12
  steer.attach(7);                       //Attaches steering servo to pin 13
  Serial.println("MOTOR ATTACHED");       //Prints a message for debug
  delay(500);
}
void brake(){
  motor.writeMicroseconds(brakeSpeed);
  delay(300);
  motor.writeMicroseconds(1500);
}
void reverse(){
  motor.writeMicroseconds(speedCenter);
  delay(100);
  motor.writeMicroseconds(brakeSpeed);
  delay(100);
  motor.writeMicroseconds(speedCenter);
  delay(100);
  motor.writeMicroseconds(reverseSpeed);
 // delay(100);
  }


void loop() {
  steer.write(neutral);
  delay(1000);
  steer.write(steerRight);
  delay(1000);
  steer.write(steerLeft);
  delay(1000);
  steer.write(neutral);
  motor.writeMicroseconds(forwardSpeed);
  delay(1000);
  brake();
  delay(1000);
  reverse();
  delay(1000);
  brake();
  delay(1000);
}
