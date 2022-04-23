/*
  Attach Interrupts

  Add counter value using attachInterrupt function. 

  The circuit:
  - Connect a push button between P3.2 and ground
    Connect another push button between P3.3 and ground
    No debouncing is implemented here. Add a debouncing capacitor if needed. 
  
  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
*/

volatile uint8_t int0Count = 0;
uint8_t int0CountOld = 0;
volatile uint8_t int1Count = 0;
uint8_t int1CountOld = 0;

#pragma save
#pragma nooverlay
void int0Callback() {
  int0Count++;
}
#pragma restore

#pragma save
#pragma nooverlay
void int1Callback() {
  int1Count++;
}
#pragma restore

void setup() {
  // put your setup code here, to run once:
  attachInterrupt(0, int0Callback, FALLING);//P3.2
  attachInterrupt(1, int1Callback, FALLING);//P3.3
}

void loop() {
  //if you see a number increases a lot when you tie the pin Low with something mechanical, it is caused by bouncing. A small capacitor between pin and GND may help.
  if (int0Count != int0CountOld) {
    int0CountOld = int0Count;
    USBSerial_print("Int0 triggered: ");
    USBSerial_println(int0Count);
    
    if (int0Count>=5){
        USBSerial_println("Int0 detached");
        detachInterrupt(0);
    }
  }
  if (int1Count != int1CountOld) {
    int1CountOld = int1Count;
    USBSerial_print("Int1 triggered: ");
    USBSerial_println(int1Count);
  }

}
