#include <SoftI2C.h>

void setup() {
  //pass SCL and SDA pins
  Wire_begin(30, 31);
}

void loop() {

  if (!Wire_scan(0x50)) {
    USBSerial_println("No response from 0x50");
    USBSerial_flush();
  } else {
    USBSerial_println("\nDump first 8 bytes of 24C32 on 0x50 addr:");
    USBSerial_flush();

    uint8_t readData[8];
    bool success;
    //read 1 byte from 0x50 addr 0
    success = Wire_readRegister16bitAddr(0x50, 0, readData, 1);
    if (success) {
      USBSerial_print("Get 1 byte from addr 0: ");
      USBSerial_println(readData[0], HEX);
      USBSerial_flush();
    } else {
      USBSerial_println("No response from 0x50");
      USBSerial_flush();
    }

    if (success) {
      //read 7 bytes from 0x50 addr 1
      success = Wire_readRegister16bitAddr(0x50, 1, readData, 7);
      if (success) {
        USBSerial_println("Get 8 bytes from addr 1:");
        for (uint8_t i = 0; i < 7; i++) {
          USBSerial_print(readData[i], HEX);
          USBSerial_print(" ");
        }
        USBSerial_println("");
        USBSerial_flush();
      } else {
        USBSerial_println("No response from 0x50");
        USBSerial_flush();
      }
    }
  }

  delay(1000);
}
