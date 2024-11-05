/*  Autonomous RC Car
    Code written by: Kyle Naumann - last edited 12 September 2018

    A few things about this code:
    - You may notice that there are if/else statements stacked within each other. I did this to "double check" readings and prevent random turns due to odd readings.
    - I did my best to organize the variables below. The steering servo positions and sonar thresholds can be changed. The thresholds are in centimeters.
    - Pseudocode has been incldued to explain what some of the code does.
    - I have disabled debugging. This can be re-enabled by going to Edit > Find > Search for "//Serial" and replace with "Serial". Be sure to un-check "Ignore Case"

    Feel free to modify any parts of this program.
*/

#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>        // Instantiate the Wire library
#include <TFLI2C.h>      // TFLuna-I2C Library v.0.1.1



int distancef;          //Necessary vars for sonar readings, setting speeds, etc.
int distancel;

TFLI2C tflI2C;
Servo motor;            //Create ESC drive motor 'motor' and steering servo 'steer'
Servo steer;


int16_t  tfDist;    // distance in centimeters
int16_t  tfAddrfront = TFL_DEF_ADR;  // Use this default I2C address 0x10
int16_t  tfAddrleft = 0x11;  // Use this default I2C address
int walldis=70;

int speedRead;          //Vars necessary to control the speed of the motor via the ESC.
int speedRemap;
int motorSpeed;
int forwardSpeed=1570;
int brakeSpeed=1100;
int reverseSpeed=1400;
int speedCenter = 1500;

const int neutral = 90;           //Steering servo positions for straight, left turns, and right turns. These can be changed and are set in degrees.
const int steerRight = 55;
const int steerLeft = 125;


void setup() {
  Serial.begin(9600);             //Begins //Serial Monitor for debugging
  Wire.begin();           // Initalize Wire library
  motor.attach(2, 1000, 2000);           //Attaches motor to pin 12
  steer.attach(7);                       //Attaches steering servo to pin 13
  //Serial.println("MOTOR ATTACHED");       //Prints a message for debug
  delay(500);
}



int lunaDistance(int16_t addr){
  if(tflI2C.getData(tfDist, addr)){
    return tfDist;
  }
  return-1;
}
void rightTurn() {              //Function called to execute a short, right turn to get away from a wall
  steer.write(steerRight);
  motor.writeMicroseconds(forwardSpeed);
  delay(50);
  steer.write(neutral);
  //Serial.println("RIGHT TURN");
}

void leftTurn() {               //Function called to execute a short, left turn to get away from a wall
  steer.write(steerLeft);
  motor.writeMicroseconds(forwardSpeed);
  delay(50);
  steer.write(neutral);
  //Serial.println("LEFT TURN");
}

void longLeftTurn() {           //Function called to execute a long, left turn when a wall is detected by the front sensor. Should be approximately 90 degrees.
  steer.write(128);
  motor.writeMicroseconds(1540);
  if((lunaDistance(tfAddrfront)<80)&&(lunaDistance(tfAddrfront)!=-1))longLeftTurn();
  steer.write(neutral);
  //Serial.println("LONG LEFT");
}

void longRightTurn() {          //Function called to execute a long, right turn when a wall is detected by the front sensor. Should be approximately 90 degrees.
  steer.write(steerRight);
  motor.writeMicroseconds(1540);
  delay(1000);
  steer.write(neutral);
  //Serial.println("LONG RIGHT");
}

void frontLeftTurn() {          //Function called to execute a 45 degree right turn when the front left sensor is triggered.
  steer.write(steerLeft);
  motor.writeMicroseconds(forwardSpeed);
  delay(90);
  steer.write(neutral);
  //Serial.println("FRONT LEFT TURN");
}

void frontRightTurn() {         //Function called to execute a 45 degree right turn when the front left sensor is triggered.
  steer.write(steerRight);
  motor.writeMicroseconds(forwardSpeed);
  delay(90);
  steer.write(neutral);
  //Serial.println("FRONT RIGHT TURN");
}
void brake(){
  motor.writeMicroseconds(brakeSpeed);
  delay(300);
  motor.writeMicroseconds(1500);
}
void leftCorrect(){
  steer.write(110);
  //delay(80);
  //int dis=lunaDistance(tfAddrleft);
  //if((dis<walldis+10)&&(dis<150)&&(dis!=-1))leftCorrect();
  delay(70);
  steer.write(neutral);
}
void rightCorrect(){
  steer.write(75);
  //delay(80);
  //int dis=lunaDistance(tfAddrleft);
  //if((dis<walldis)&&(dis!=-1))rightCorrect();
  delay(70);
  steer.write(neutral);
}
bool start=true;
void loop() {
  if(start){
    steer.write(neutral);
    motor.writeMicroseconds(1500);
    start=false;
  }
  steer.write(neutral);
  motor.writeMicroseconds(1580);
  distancef=lunaDistance(tfAddrfront);
  distancel=lunaDistance(tfAddrleft);
  Serial.println(String(distancef)+":"+String(distancel));                            //is present before turning. This has a negligible impact on reading performance, and I used a similar structure for the readSidesAndFront() function above.
  if ((distancef<80)&&(distancef!=-1)) {
    brake();
    longLeftTurn();
   // delay(40);
   // exit(0);
  }
  if((distancel<walldis)&&(distancel!=-1)) {
    rightCorrect();
    //distancel=lunaDistance(tfAddrleft);
  }
  if((distancel>walldis+10)&&(distancel<150)&&(distancel!=-1)) {
    leftCorrect();
    //distancel=lunaDistance(tfAddrleft);
  }
  steer.write(neutral);
  if((distancel>200)&&(distancef>80)&&(distancel!=-1)){
    brake();
    longLeftTurn();
    delay(40);
  }
}
