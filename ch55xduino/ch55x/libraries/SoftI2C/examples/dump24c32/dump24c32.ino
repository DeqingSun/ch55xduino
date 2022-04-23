#include <SoftI2C.h>

void setup() {
  scl_pin = 30; //extern variable in SoftI2C.h
  sda_pin = 31;
  I2CInit();
}

void loop() {

  uint8_t ack_bit, readData1, readData2;

  USBSerial_println("\nDump first 8 bytes of 24C32 on 0x50 addr:");
  USBSerial_flush();
  I2CStart();
  ack_bit = I2CSend(0x50 << 1 | 1); //last bit is r(1)/w(0).
  I2CStop();

  if (ack_bit == 0) {
    for (uint8_t i = 0; i < 8; i += 2) {
      I2CStart();
      ack_bit = I2CSend(0x50 << 1 | 0); //last bit is r(1)/w(0).
      if (ack_bit != 0) break;
      ack_bit = I2CSend(0); //high EEPROM addr
      if (ack_bit != 0) break;
      ack_bit = I2CSend(i); //low EEPROM addr
      if (ack_bit != 0) break;
      I2CRestart();
      ack_bit = I2CSend(0x50 << 1 | 1); //last bit is r(1)/w(0).
      if (ack_bit != 0) break;
      readData1 = I2CRead();
      I2CAck();
      readData2 = I2CRead();
      I2CNak();
      I2CStop();
      USBSerial_print("Get 2 bytes start from addr ");
      USBSerial_flush();
      USBSerial_print(i, HEX);
      USBSerial_print(": ");
      USBSerial_print(readData1, HEX);
      USBSerial_print(", ");
      USBSerial_println(readData2, HEX);
      USBSerial_flush();
    }

  } else {
    USBSerial_println("No response from 0x50");
    USBSerial_flush();
  }

  delay(1000);

}
