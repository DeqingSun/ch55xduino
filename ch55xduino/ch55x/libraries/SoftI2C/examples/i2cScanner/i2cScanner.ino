#include <SoftI2C.h>

void setup() {
  scl_pin = 30; //extern variable in SoftI2C.h
  sda_pin = 31;
  I2CInit();
}

void loop() {

  uint8_t ack_bit;

  USBSerial_println("\nScanning:");

  for (uint8_t i = 0; i < 128; i++) {
    I2CStart();
    ack_bit = I2CSend(i << 1 | 1); //last bit is r(1)/w(0).
    I2CStop();
    delay(1);
    if (ack_bit == 0) {
      USBSerial_print("I2C got ACK from: 0x");
      USBSerial_println(i, HEX);
    }
  }
  delay(1000);

}
