#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define cali  1000
#define mfilter  8
Adafruit_MPU6050 mpu;


double rads = 0;           //Steering
double xVel = 0, yVel = 0, zVel = 0;
double xPos = 0, yPos = 0, zPos = 0;
double ctim;
double dtime;
int stime = 8;
int setupMPUdelay = 40000;

double gxOffset = 0, gyOffset = 0, gzOffset = 0;
double axOffset = 0, ayOffset = 0, azOffset = 0;
double xaccel[mfilter]={0},yaccel[mfilter]={0},zaccel[mfilter]={0};
double filtxaccel,filtyaccel,filtzaccel;
float thresh = 0.04;

void filtaccel(){
  filtxaccel=0;
  filtyaccel=0;
  filtzaccel=0;
  for (int i=mfilter; i>=0;i--){
       filtxaccel+=xaccel[i];
       filtyaccel+=yaccel[i];
       filtzaccel+=zaccel[i];
  }
  filtxaccel/=mfilter;
  filtyaccel/=mfilter;
  filtzaccel/=mfilter;
  
}
void axarrshift(){
  for (int i=mfilter; i>0;i--){
     xaccel[i]=xaccel[i-1];
     yaccel[i]=yaccel[i-1];
     zaccel[i]=zaccel[i-1];
  }
}

void mpuCalibrateOffsets(){
  delay(setupMPUdelay);
  //Mpu calibration
  sensors_event_t aOffset, gOffset, tempOffset;
  gxOffset = 0.0;
  gyOffset = 0.0;
  gzOffset = 0.0;
  axOffset = 0.0;
  ayOffset = 0.0;
  azOffset = 0.0;
  
  //Calibrate gyro
  for(int i = 0; i < cali; i++){
    mpu.getEvent(&aOffset, &gOffset, &tempOffset);
    gxOffset += gOffset.gyro.x;
    gyOffset += gOffset.gyro.y;
    gzOffset += gOffset.gyro.z;
  }
  gxOffset /= cali;
  gyOffset /= cali;
  gzOffset /= cali;

  //Calibrate accel
  for(int i = 0; i < cali; i++){
    mpu.getEvent(&aOffset, &gOffset, &tempOffset);
    axOffset += aOffset.acceleration.x;
    ayOffset += aOffset.acceleration.y;
    azOffset += aOffset.acceleration.z;
  }
  axOffset /= cali;
  ayOffset /= cali;
  azOffset /= cali;
}

void setup(void) {
	Serial.begin(115200);
  Serial.setTimeout(1);
  mpu.begin();
	mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
	mpu.setGyroRange(MPU6050_RANGE_500_DEG);
	mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpuCalibrateOffsets();

	delay(100);
  ctim = millis();
}

void loop() {
  dtime = millis()-ctim;
  if (dtime>stime){
    sensors_event_t a, g, temp;               //Gyro var
  
    //steer.write(steerRight);
    //motor.writeMicroseconds(forwardSpeed);
    mpu.getEvent(&a, &g, &temp);
    ctim = millis();
    axarrshift(); //shift the array
    xaccel[0] = a.acceleration.x - axOffset;
    yaccel[0] = a.acceleration.y - ayOffset;
    zaccel[0]= a.acceleration.z - azOffset;
    filtaccel();//filter the array
    //Rotation
    rads += (g.gyro.x - gxOffset) * ((1.0 * (dtime )));
    if(abs(filtxaccel)>thresh){
      //Velocity
      xVel += filtxaccel * ((1.0 * (dtime)));
      //Distance
      xPos += (xVel) * ((1.0 * (dtime)));
      }

    if(abs(filtyaccel)>thresh){
      //Velocity
      yVel += filtyaccel * ((1.0 * (dtime)));
      //Distance
      yPos += (yVel) * ((1.0 * (dtime)));
      }
    if(abs(filtzaccel)>thresh){
      //Velocity
      zVel += filtzaccel * ((1.0 * (dtime)));
      //Distance
      zPos += (zVel) * ((1.0 * (dtime)));
      }
    
    //delay(8);
  
    // if(rads > 20){
    //   steer.write(neutral);
    //   delay(200);
    //   exit(0);
    // }
  
    Serial.println(/*"Rads: " + (String)*/rads);
    //Serial.println("XYZ Gyro: " + (String)g.gyro.x + ", " + (String)g.gyro.y + ", " + (String)g.gyro.z);
    //Serial.println("XYZ Accel: " + (String)a.acceleration.x + ", " + (String)a.acceleration.y + ", " + (String)a.acceleration.z);
    Serial.println("XYZ Accel: " + (String)(a.acceleration.x - axOffset) + ", " + (String)(a.acceleration.y - ayOffset) + ", " + (String)(a.acceleration.z - azOffset));
    Serial.println("XYZ Vel: " + (String)xVel + ", " + (String)yVel + ", " + (String)zVel);
    //Serial.println("XYZ Pos: " + (String)xPos + ", " + (String)yPos + ", " + (String)zPos);
  }
}
