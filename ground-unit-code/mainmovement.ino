/*
 This sketch shows to integrate acceleration to obtain an estimate of velocity
*/

/* NOTE: 
   1. The accelerometer MUST be calibrated to integrate its output. Adjust the 
   AX, AY, AND AZ offsets until the sensor reads (0,0,GRAVITY) at rest.

   2. REQUIRED UNITS:
      I. The gyroscope must output: radians/second
      II. The accelerometer output:  length/second^2  [The length unit is arbitrary. Can be meter, feet, etc]
*/

#include <basicMPU6050.h>   // Library for IMU sensor. See this link: https://github.com/RCmags/basicMPU6050
#include <accIntegral.h>
#include <imuFilter.h>
#include <ADNS3080.h>
//motor
#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>        // Instantiate the Wire library
#include <TFLI2C.h>  
#include <JY901.h>


int distancef;          //Necessary vars for sonar readings, setting speeds, etc.
int distancel;

TFLI2C tflI2C;
Servo motor;            //Create ESC drive motor 'motor' and steering servo 'steer'
Servo steer;


int16_t  tfDist;    // distance in centimeters
int16_t  tfAddrfront = TFL_DEF_ADR;  // Use this default I2C address 0
int16_t  tfAddrleft = 0x11;  // Use this default I2C address
int walldis = 70;

int speedRead;          //Vars necessary to control the speed of the motor via the ESC.
int speedRemap;
int motorSpeed;
int forwardSpeed=1570;
int turnSpeed=1550;
int brakeSpeed=1001;
int reverseSpeed=1400;
int speedCenter = 1500;
int reverse_time;

const int neutral = 105;           //Steering servo positions for straight, left turns, and right turns. These can be changed and are set in degrees.
const int steerRight = 55;
const int steerLeft = 125;


double base;
 
  uint8_t  motion;      // Did motion occur? true or false
  int8_t   dx, dy;      // Displacement since last function call
  uint8_t  squal;       // Surface quality value - Larger value means better tracking. If below ~15 the displacement is dissabled. Adjust focus for 30 or more.
  uint16_t shutter;     // Camera shutter rate in clock cycles.
  uint8_t  max_pixel;   // Maximum pixel value of current frame (max brightness)

// SPI pins
#define PIN_RESET     9         
#define PIN_CS        53       

// sensor configuration 
#define LED_MODE      false     // If true, enables LED Mode
#define RESOLUTION    false     // If true, enable high resolution mode 

ADNS3080 <PIN_RESET, PIN_CS> sensor;
double DPI=5.869;
// Initial position
int x = 0;
int y = 0;
double disx=0;
double disy=0;
// ========== IMU sensor ==========

// Gyro settings:
#define         LP_FILTER   3           // Low pass filter.                    Value from 0 to 6
#define         GYRO_SENS   0           // Gyro sensitivity.                   Value from 0 to 3
#define         ACCEL_SENS  0           // Accelerometer sensitivity.          Value from 0 to 3
#define         ADDRESS_A0  LOW         // I2C address from state of A0 pin.   A0 -> GND : ADDRESS_A0 = LOW
                                        //                                     A0 -> 5v  : ADDRESS_A0 = HIGH
// Accelerometer offset:
constexpr int   AX_OFFSET =  552;       // Use these values to calibrate the accelerometer. The sensor should output 1.0g if held level. 
constexpr int   AY_OFFSET = -241;       // These values are unlikely to be zero.
constexpr int   AZ_OFFSET = -3185;

//-- Set template parameters:
#define GAIN          0.5     /* Fusion gain, value between 0 and 1 - Determines orientation correction with respect to gravity vector. 
                                 If set to 1 the gyroscope is dissabled. If set to 0 the accelerometer is dissabled (equivant to gyro-only) */

#define SD_ACCEL      0.2
basicMPU6050<LP_FILTER,  GYRO_SENS,  ACCEL_SENS, ADDRESS_A0,
             AX_OFFSET,  AY_OFFSET,  AZ_OFFSET
            >imu;

// =========== Settings ===========
accIntegral fusion;
imuFilter filt;

// Filter coefficients               //  Unit           
constexpr float GRAVITY = 9.81e3;    //  mm/s^2    Magnitude of gravity at rest. Determines units of velocity. [UNITS MUST MATCH ACCELERATION]
constexpr float SD_ACC  = 1000;      //  mm/s^2    Standard deviation of acceleration. Deviations from zero are suppressed.
constexpr float SD_VEL  = 200;       //  mm/s      Standard deviation of velocity. Deviations from target value are suppressed.
constexpr float ALPHA   = 0.5;       //            Gain of heading update - See example "output" for more information.
// Sensor scaling
constexpr float TO_LENGTH_PER_SECOND_SQ = GRAVITY;     // Constant to convert acceleration measurements to:  length/second^2
constexpr float TO_RAD_PER_SECOND = 1.0;               // Constant to convery gyroscope measurements to:     radians/second

int direction=3;
double angle=0;
double baseoffang=19/180;

void setup() {
  Serial.begin(115200);

  while (!Serial1) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial1.begin(250000);
  delay(100);
  sensor.setup( LED_MODE, RESOLUTION );//adns3080

  Wire.begin();//lidar connect
  // mpu6050.begin();
  // mpu6050.calcGyroOffsets(true);
  // calibrate IMU sensor ie mp6050
  imu.setup();
  imu.setBias();
  // fusion.setup();

  // // Initialize filter: 
  fusion.setup( imu.ax(), imu.ay(), imu.az() );     
  fusion.rotateHeading( 90*DEG_TO_RAD, SMALL_ANGLE );   // Can choose LARGE_ANGLE or SMALL_ANGLE approximation

  steer.attach(7);  //steering
  motor.attach(2, 1000, 2000);           //Attaches motor to pin 2
	
  delay(100);                            
}
int lunaDistance(int16_t addr){
  if(tflI2C.getData(tfDist, addr)){
    return tfDist;
  }
  return-1;
}
int directionAng(){
  //given driection %4 it will output the proper angle in radians
  if(direction%4==0) return 0;
  else if(direction%4==1) return 270;
  else if(direction%4==2) return 180;
  else if(direction%4==3) return 90;
  else return-1;
}
void updateAngle(){
    // mpu6050.update();
    // angle=mpu6050.getAngleZ(); 
    
    fusion.update( imu.gx(), imu.gy(), imu.gz(), imu.ax(), imu.ay(), imu.az() ,ALPHA, SD_ACC, GRAVITY);    

    // imu.updateBias(); 
    // vec3_t accel = { imu.ax(), imu.ay(), imu.az() };
    // vec3_t gyro = { imu.gx(), imu.gy(), imu.gz() };

    // accel *= TO_LENGTH_PER_SECOND_SQ;
    // gyro *= TO_RAD_PER_SECOND;
    // vec3_t vel_t = {0,0,0}; 
    // fusion.update( gyro, accel, vel_t, SD_ACC, SD_VEL, GRAVITY, ALPHA ); 
  
    angle=fusion.yaw()*RAD_TO_DEG;
    // if(offang==0){
    //   offang=angle;
    // }
    // angle=angle-offang;
    // angle+=90;


}
void updateMovement(){
  updateAngle();
  // sensor.motionBurst( &motion, &dx, &dy, &squal, &shutter, &max_pixel );
  // x += -1*dx*cos(angle*DEG_TO_RAD);
  // y += -1*dy*sin(angle*DEG_TO_RAD);
  // disx=x/DPI;
  // disy=y/DPI;
}

// void updateMovement2(){
//   updateAngle();
//   sensor.motionBurst( &motion, &dx, &dy, &squal, &shutter, &max_pixel );
//   x += -1*dx*cos(angle*DEG_TO_RAD);
//   y += -1*dy*sin(angle*DEG_TO_RAD);
//   disx=x/DPI;
//   disy=y/DPI;
// }
void brake(){
  motor.writeMicroseconds(brakeSpeed);
  unsigned long start_time = millis(); // Get the current time
  // Wait until 500 milliseconds have passed
  while (millis() - start_time < 500) {
    updateMovement();
  }
  //delay(500);
  motor.writeMicroseconds(1500);
}
void reversebrake(){
  motor.writeMicroseconds(1700);
  delay(100);
  motor.writeMicroseconds(1500);
}
void leftCorrect(double keepangle){
  for(int i=neutral+1;i<neutral-20;i++){
    // double gyangle = angleCalc();
    steer.write(i);
    delay(5);
    if(angle>keepangle-2){
      break;
    }
  }
}
void rightCorrect(double keepangle){
    for(int i=neutral-1;i>neutral-20;i--){
    // double gyangle = angleCalc();
    steer.write(i);
    delay(5);
    if(angle<keepangle+2){
      break;
    }
  }
}
void keepStraight(double keepangle){
  // double gyangle = angleCalc();
  if(angle<keepangle-2){
    leftCorrect(keepangle);
  }
  else if(angle>keepangle+2){
    rightCorrect(keepangle);
  }
  else steer.write(neutral);
}
void turnAngle(double turnangle){
  double base=angle+turnangle;
  if(turnangle>0){
    base=base-baseoffang*angle;
    steer.write(steerLeft);
    motor.writeMicroseconds(turnSpeed);
    while(angle<base){
      updateMovement();            
    } 
    brake(); 
    steer.write(neutral);        
  }
  else if(turnangle<0){
    base=base+baseoffang*angle;
    steer.write(steerRight);
    motor.writeMicroseconds(turnSpeed);
    while(angle>base){
      updateMovement();    
    } 
    brake();  
    steer.write(neutral);
  }
}
void turnLeft90(){//function to turn right 90 degrees
  turnAngle(90);
  // double angleturn=angle+90;
  // if(angleturn>180){
  //   int temp=angleturn-180;
  //   angleturn=-180+temp;
  // }
  // motor.writeMicroseconds(1560);
  // while(angle<angleturn){
  //   steer.write(steerLeft);
  // }
  // steer.write(neutral);
  direction++;  
}
void turnRight90(){//function to turn right 90 degrees
  turnAngle(-90);
  // double angleturn=angle-90;
  // if(angleturn>180){
  //   int temp=angleturn-180;
  //   angleturn=-180+temp;
  // }
  // motor.writeMicroseconds(1560);
  // while(angle>angleturn){
  //   steer.write(steerRight);
  //   updateMovement();
  // }
  // steer.write(neutral);
  direction--;
}
void moveForward(int distance){
  float startdis=disx+disy;
  float movement=disx+disy;
  updateMovement();
  double forwardangle=angle;
  // updateAngle();
  while(abs(movement-startdis)<distance){
    keepStraight(forwardangle);
    motor.writeMicroseconds(forwardSpeed);
    updateMovement();
    // updateAngle();
    movement=disx+disy;
  }
  brake();
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
void shimyLeft(){
  double base=angle;
  steer.write(steerLeft);
  reverse();
  delay(800);
  steer.write(steerRight);
  reverse();
  while(angle<base-0.5){}
  reversebrake();
  //delay(600);
  steer.write(neutral);
  delay(100);
}
void shimyRight(){
  base=angle;
  steer.write(steerRight);
  reverse();
  delay(800);
  steer.write(steerLeft);
  reverse();
  while(angle>base+0.5){}
  reversebrake();
  //delay(550);
  steer.write(neutral);
  delay(100);
}
void x180(int ypos, int xpos,int xdis,int ydis){
  if(ypos<0){
    turnLeft90();
    moveForward(ypos-(ydis-y));        
  }
  else if(ypos>0){
    turnRight90();
    moveForward(ypos-(ydis-y));
  }
}
void x0(int ypos, int xpos,int xdis,int ydis){
  if(ypos>0){
    turnLeft90();
    moveForward(ypos-(ydis-y));        
  }
  else if(ypos<0){
    turnRight90();
    moveForward(ypos-(ydis-y));
  }
}
void y90(int ypos, int xpos,int xdis,int ydis){
  if(xpos<0){
    turnLeft90();
    moveForward(xpos-(x-xdis));        
  }
  else if(xpos>0){
    turnRight90();
    moveForward(xpos-(x-xdis));  
  }
}
void y270(int ypos, int xpos,int xdis,int ydis){
  if(xpos>0){
    turnLeft90();
    moveForward(xpos-(x-xdis));        
  }
  else if(xpos<0){
    turnRight90();
    moveForward(xpos-(x-xdis));  
  }  
}
void goto_cmd(int xpos, int ypos){
  int xdis=x;
  int ydis=y;
  int dirang=directionAng();
  if(dirang==0){
    if(xpos<0){
      if(ypos>0){
        turnLeft90();                        
        moveForward(ypos-(y-ydis));  
        y90(ypos, xpos, xdis, ydis);             
      }
      else if(ypos<0){
        turnRight90();                        
        moveForward(ypos-(y-ydis));
        y270(ypos, xpos, xdis, ydis);
      }
    }
    else if(xpos>0){      
      moveForward(xpos); 
      x0(ypos, xpos, xdis, ydis);
    }
  }
  else if(dirang==90){
    if(ypos<0){
      if(xpos<0){
        turnLeft90();                        
        moveForward(xpos-(x-xdis));  
        x180(ypos, xpos, xdis, ydis);           
      }
      else if(xpos>0){
        turnRight90();                        
        moveForward(xpos-(x-xdis));
        x0(ypos, xpos, xdis, ydis);
      } 
    }
    else if(ypos>0){      
      moveForward(ypos); 
      y90(ypos, xpos, xdis, ydis);
    }
  }
  else if(dirang==180){
    if(xpos>0){
      if(ypos<0){
        turnLeft90();                        
        moveForward(ypos-(y-ydis));  
        y270(ypos, xpos, xdis, ydis);             
      }
      else if(ypos>0){
        turnRight90();                        
        moveForward(ypos-(y-ydis));
        y90(ypos, xpos, xdis, ydis);
      }
    }
    else if(xpos<0){      
      moveForward(xpos); 
      x180(ypos, xpos, xdis, ydis);
    }
  }
  else if(dirang==270){
    if(ypos>0){
      if(xpos>0){
        turnLeft90();                        
        moveForward(xpos-(x-xdis));  
        x0(ypos, xpos, xdis, ydis);           
      }
      else if(xpos<0){
        turnRight90();                        
        moveForward(xpos-(x-xdis));
        x180(ypos, xpos, xdis, ydis);
      } 
    }
    else if(ypos<0){      
      moveForward(ypos); 
      y270(ypos, xpos, xdis, ydis);
    }
  }
}
double convert_angle(double angle_degrees) {
    double angle_normalized = angle_degrees;

    // Normalize angle to be between 0 and 360 degrees
    while (angle_normalized < 0) {
        angle_normalized += 360;
    }

    while (angle_normalized >= 360) {
        angle_normalized -= 360;
    }

    return angle_normalized;
}
string cmd="";
bool exe=false;
int index=0;
int n=0;
String str="";
int grabIntString(String input){
  int out=0;
  int index=0;
  bool neg=false;
  for(int i=0;i<input.length();i++){
    if(isDigit(input[i])){
      out=out+(input[i]-48)*(pow(10,index));
      index++;
    }
    if(index==1){
      if(input[i]=='-'){
        neg=true;
      }
    }
  }
  if(neg)out=out*-1;
  return out;
}
void loop() { 
  updateMovement();
  if (Serial1.available() > 0) { // Check if data is available to read
    str = Serial1.readStringUntil(10); // Read the incoming data 
    if (str.length()>0) {
      cmd = str.substring(0,str.indexOf(','));
      str = str.substring(parse.length()+1);
      n=grabIntString(cmd);
      cmd.trim();
      if(cmd.equals("go")){
        motor.writeMicroseconds(forwardSpeed);
      }
      else if(cmd.equals("stop"){
        int spe=motor.readMicroseconds();
        if(spe<1500)reverseBrake();
        else brake();
        Serial1.println("stop success");
      }
      else if(cmd.equals("goAngle"+String(n)){
        turnAngle(n);
      }
      else if(cmd.equals("reverse"){
        motor.writeMicroseconds(reverseSpeed);
      }
      else {
      }
      cmd="";
      n=0;
    }
    else cmd=cmd+incomingData;
  }
  // steer.write(neutral);
  // motor.writeMicroseconds(1500);
  // delay(50);
  // updateMovement();
  // //wait for drone to takeoff
  // moveForward(100);
  // delay(1000);
  // goto_cmd(300,100);
  // // turnAngle(90);
  // // delay(100);
  // // turnAngle(-90);
  // // delay(5000);
  // exit(0);
  //while photresistor is bright move foward, once dark, waitr for movement send from nano
    //move out of box
  //if touch and go wait for land and then takeoff
  //wait for signal send from nano on box position  

  //if coordinate is (300,400)
  // moveForward(350);
  // brake();
  // turnLeft90();
  // brake();
  // moveForward(250);

  // //testing
  // moveForward(350);
  // delay(3000);
  // turnRight90();

  // delay(100);
  // turnLeft90();
  // exit(0);
  // steer.write(70);
  // motor.writeMicroseconds(1500);
  // // delay(2000);
  // steer.write(100);
  // motor.writeMicroseconds(1100);
  // shimyRight();
  // brake();
  // shimyLeft();
  // delay(2000);
  // motor.writeMicroseconds(1500);
  // delay(1000);




  // // delay(2000);
  // shimyLeft();
  // delay(800);
  // shimyRight();
  // delay(4000);
  // shimyRight();
}
// void serialEvent1() 
// {
//   while (Serial1.available()) 
//   {

// ///////////////////////////////
//     // JY901.CopeSerialData(Serial1.read()); //Call JY901 data cope function
  
//     // interval = (millis() - preInterval) * 0.001;

//     // angle += ((float)JY901.stcGyro.w[2]/32768*2000)*interval;

//     // preInterval = millis();
// //////////////////////////

//     JY901.CopeSerialData(Serial1.read()); //Call JY901 data cope function
//     anglebl=(float)JY901.stcAngle.Angle[2]/32768*180; 
//     if(offangbl==0){
//       offangbl=anglebl;
//     }
//     anglebl=anglebl-offang;

//     mpu6050.update();
//     angle=mpu6050.getAngleZ(); 

    
//     // imu.updateBias(); 
//     // vec3_t accel = { imu.ax(), imu.ay(), imu.az() };
//     // vec3_t gyro = { imu.gx(), imu.gy(), imu.gz() };

//     // accel *= TO_LENGTH_PER_SECOND_SQ;
//     // gyro *= TO_RAD_PER_SECOND;
//     // vec3_t vel_t = {0,0,0}; 
//     // fusion.update( gyro, accel, vel_t, SD_ACC, SD_VEL, GRAVITY, ALPHA ); 
  
//     // angle=fusion.yaw()*RAD_TO_DEG;
//     if(offang==0){
//       offang=angle;
//     }
//     angle=angle-offang;
//     angle+=90;
//     // uint8_t  motion;      // Did motion occur? true or false
//     // int8_t   dx, dy;      // Displacement since last function call
//     // uint8_t  squal;       // Surface quality value - Larger value means better tracking. If below ~15 the displacement is dissabled. Adjust focus for 30 or more.
//     // uint16_t shutter;     // Camera shutter rate in clock cycles.
//     // uint8_t  max_pixel;   // Maximum pixel value of current frame (max brightness)

//     sensor.motionBurst( &motion, &dx, &dy, &squal, &shutter, &max_pixel );
//     x += -1*dx*cos(angle*DEG_TO_RAD);
//     y += -1*dy*sin(angle*DEG_TO_RAD);
//     disx=x/DPI;
//     disy=y/DPI;
//   }
// }
