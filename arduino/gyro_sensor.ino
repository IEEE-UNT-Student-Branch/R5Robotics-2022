#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

double rads = 0;           //Steering
double xVel = 0, yVel = 0, zVel = 0;
double xPos = 0, yPos = 0, zPos = 0;
int time;
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

	// Try to initialize!
	if (!mpu.begin()) {
		Serial.println("Failed to find MPU6050 chip");
		while (1) {
		  delay(10);
		}
	}
	Serial.println("MPU6050 Found!");

	// set accelerometer range to +-8G
	mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

	// set gyro range to +- 500 deg/s
	mpu.setGyroRange(MPU6050_RANGE_500_DEG);

	// set filter bandwidth to 21 Hz
	mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

	delay(100);
  time = millis();
}

void loop() {
  sensors_event_t a, g, temp;               //Gyro var

  //steer.write(steerRight);
  //motor.writeMicroseconds(forwardSpeed);
  mpu.getEvent(&a, &g, &temp);
  //Rotation
  rads += (g.gyro.x - gxOffset) * ((1.0 * (time - millis())) / 100000000000.0);
  //Velocity
  //xVel += (a.acceleration.x - axOffset) * ((1.0 * (time - millis())) / 1000000000.0);
  ////yVel += (a.acceleration.y - ayOffset) * ((1.0 * (time - millis())) / 1000000000.0);
  zVel += (a.acceleration.z - azOffset) * ((1.0 * (time - millis())) / 1000000000.0);
  //Distance
  //xPos += (xVel) * ((1.0 * (time - millis())) / 1000000000.0);
  //yPos += (yVel) * ((1.0 * (time - millis())) / 1000000000.0);
  //zPos += (zVel) * ((1.0 * (time - millis())) / 1000000000.0);   
  time = millis();

  // if(rads > 20){
  //   steer.write(neutral);
  //   delay(200);
  //   exit(0);
  // }

  //Serial.println("Rads: " + (String)rads);
  //Serial.println("XYZ Accel: " + (String)a.acceleration.x + ", " + (String)a.acceleration.y + ", " + (String)a.acceleration.z);
  //Serial.println("XYZ Accel: " + (String)(a.acceleration.x - axOffset) + ", " + (String)(a.acceleration.y - ayOffset) + ", " + (String)(a.acceleration.z - azOffset));
  Serial.println("XYZ Vel: " + (String)xVel + ", " + (String)yVel + ", " + (String)zVel);
  //Serial.println("XYZ Pos: " + (String)xPos + ", " + (String)yPos + ", " + (String)zPos);

}