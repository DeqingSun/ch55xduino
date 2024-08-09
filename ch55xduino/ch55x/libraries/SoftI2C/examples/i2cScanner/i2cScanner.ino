#include <SoftI2C.h>

void setup() {
  //pass SCL and SDA pins
  Wire_begin(30, 31);
}

void loop() {

  USBSerial_println("\nScanning:");

  for (uint8_t i = 0; i < 128; i++) {
    delay(1);
    if (Wire_scan(i)) {
      USBSerial_print("I2C got ACK from: 0x");
      USBSerial_println(i, HEX);
    }
  }

  delay(1000);
}
