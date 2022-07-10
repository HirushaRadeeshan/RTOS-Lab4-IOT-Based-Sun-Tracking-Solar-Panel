#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
int rotate_angle =0;
int servoPin = 18;

void setup() {
  // put your setup code here, to run once:
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
  myservo.write(0);

}

void loop() {
  // put your main code here, to run repeatedly:

  myservo.write(85);

}
