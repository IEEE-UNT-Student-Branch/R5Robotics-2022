/* Servo motor with Arduino example code. Position and sweep. More info: https://www.makerguides.com/ */

// Include the servo library:
#include <Servo.h>

// Create a new servo object:
Servo myservo;

// Define the servo pin:
#define servoPin1 2
// #define servoPin0 A0

// Create a variable to store the servo position:
float angle = 0;
float max = 100;
float min = 97;
int i = 0;

void servo_angle(void){
    Serial.print(angle);
    Serial.print('\n');

  // if(angle%5==0){
  //   Serial.print(angle);
  //   Serial.print('\n');
// }
}

void ramp_up(void) {
    for (angle = 95; angle <= 98.9; angle += 0.1) {
    myservo.write(angle);
    delay(1);
    servo_angle();
    delay(50);
  }
}

void move_forward(void) {
  for (angle = 98; angle <= 99; angle += 0.1) {
    myservo.write(angle);
    delay(100);
    servo_angle();
    delay(500);
  }

  // And back from 180 to 0 degrees:
for (angle = 99; angle >= 98; angle -= 0.1) {
    myservo.write(angle);
    delay(500);
    servo_angle();
  }
  delay(10);
}

void slow_down(void) {
  for (angle = 94; angle <= 90; angle -= 0.5) {
    myservo.write(angle);
    delay(100);
    servo_angle();
    delay(500);
  }  
}

void setup() {
  Serial.begin(9600);
  cli();
  OCR4A=0;  
  OCR4B=0;  
  OCR4C=0;  
  TCCR4A = 0;
  TCCR4B = 0;
  TCCR4C = 0;
  TCNT4 = 0;
  TCCR4A |= (1 << COM4B0);
  TCCR4B |= (1 << WGM42);
  TCCR4B |= (1 << CS40);
  TCCR4B |= (1 << CS41);
  OCR4A = 62499; // (16 MHz / 256) / 1 Hz = 62500
  TIMSK4 |= (1 << OCIE4B);
  sei();
  // Attach the Servo variable to a pin:
  myservo.attach(servoPin1);
  Serial.begin(9600);
  // myservo.attach(servoPin0);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Serial is working!");
}

ISR(TIMER4_COMPB_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
TCNT4 = 0;

//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
 Serial.println("Hello World");
}

void loop() {
  ramp_up();
  move_forward();

  }






