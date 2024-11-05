#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define sampTime 8
#define samps 1

Adafruit_MPU6050 mpu;

double rads = 0;           //Steering
double xVel = 0, yVel = 0, zVel = 0;
double xPos = 0, yPos = 0, zPos = 0;
int ctime;
int dtime;
int i;
double ggyro=0,xaccel=0,yaccel=0,zaccel=0;

int dsamp = sampTime/samps;
double gxOffset = 0, gyOffset = 0, gzOffset = 0;
double axOffset = 0, ayOffset = 0, azOffset = 0;

void mpuCalibrateOffsets(){
  //Mpu calibration
  sensors_event_t aOffset, gOffset, tempOffset;
  gxOffset = 0.0;
  gyOffset = 0.0;
  gzOffset = 0.0;
  axOffset = 0.0;
  ayOffset = 0.0;
  azOffset = 0.0;
  
  //Calibrate gyro
  for(int i = 0; i < 50; i++){
    mpu.getEvent(&aOffset, &gOffset, &tempOffset);
    gxOffset += gOffset.gyro.x;
    gyOffset += gOffset.gyro.y;
    gzOffset += gOffset.gyro.z;
  }
  gxOffset /= 50.0;
  gyOffset /= 50.0;
  gzOffset /= 50.0;

  //Calibrate accel
  for(int i = 0; i < 50; i++){
    mpu.getEvent(&aOffset, &gOffset, &tempOffset);
    axOffset += aOffset.acceleration.x;
    ayOffset += aOffset.acceleration.y;
    azOffset += aOffset.acceleration.z;
  }
  axOffset /= 50.0;
  ayOffset /= 50.0;
  azOffset /= 50.0;
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
  ctime = millis();
}

void loop() {
  //Serial.println(dtime);
  sensors_event_t a, g, temp;               //Gyro var
  dtime = millis() - ctime;
  if (dtime > dsamp){
    Serial.println(dtime);
    i+=1;
    //steer.write(steerRight);
    //motor.writeMicroseconds(forwardSpeed);
    mpu.getEvent(&a, &g, &temp);
      xaccel += a.acceleration.x-axOffset;
      yaccel += a.acceleration.y-ayOffset;
      zaccel += a.acceleration.z-azOffset;
      ggyro += g.gyro.x - gxOffset;
    if (i>=samps){
      i=0;
      //Rotation
      rads += ggyro/samps * (1.0 * dtime) / 1000000000.0;
      //Velocity
      xVel += xaccel/samps * (1.0 * dtime) / 1000000000.0;
      yVel += yaccel/samps * (1.0 * dtime) / 1000000000.0;
      zVel += zaccel/samps * (1.0 * dtime) / 1000000000.0;
      //Distance
      xPos += 100*(xVel) * (1.0 * dtime) / 1000000000.0;
      yPos += (yVel) * (1.0 * dtime) / 1000000000.0;
      zPos += (zVel) * (1.0 * dtime) / 1000000000.0;
      ctime = millis();
      //delay(8);
    
      // if(rads > 20){
      //   steer.write(neutral);
      //   delay(200);
      //   exit(0);
      // }
    
      Serial.println(/*"Rads: " + (String)*/rads);
      //Serial.println("XYZ Gyro: " + (String)g.gyro.x + ", " + (String)g.gyro.y + ", " + (String)g.gyro.z);
      //Serial.println("XYZ Accel: " + (String)a.acceleration.x + ", " + (String)a.acceleration.y + ", " + (String)a.acceleration.z);
      //Serial.println("XYZ Accel: " + (String)(a.acceleration.x - axOffset) + ", " + (String)(a.acceleration.y - ayOffset) + ", " + (String)(a.acceleration.z - azOffset));
      //Serial.println("XYZ Vel: " + (String)xVel + ", " + (String)yVel + ", " + (String)zVel);
      Serial.println("XYZ Pos: " + (String)xPos + ", " + (String)yPos + ", " + (String)zPos);
    }
  }
}
