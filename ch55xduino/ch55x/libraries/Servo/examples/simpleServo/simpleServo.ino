#include <Servo.h>

void setup() {
  Servo_init();
  pinMode(12, OUTPUT);
  Servo_attach(12);
  pinMode(13, OUTPUT);
  Servo_attach(13);
}

void loop() {
  Servo_writeMicroseconds(12, 1000);
  Servo_write(13,0);
  delay(3000);
  Servo_writeMicroseconds(12, 1500);
  Servo_write(13,90);
  delay(3000);
  Servo_writeMicroseconds(12, 2000);
  Servo_write(13,180);
  delay(3000);
}
