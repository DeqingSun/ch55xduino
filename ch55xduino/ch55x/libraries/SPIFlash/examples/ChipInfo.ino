#include "SPIFlash.h"

// Read chip metadata information example

void setup() {  
  
  initialize();

  while (!USBSerial()) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  USBSerial_println("*** Accessing W25QXX flash chip via SPI. ***");

  // Device id
  uint16_t deviceId = readDeviceId();
  USBSerial_print("DeviceID: ");
  USBSerial_println(deviceId, HEX);

  // Unique ID
  uint8_t *p = readUniqueId();

  USBSerial_print("UniqueID: ");
  for (uint8_t i=0;i<8;i++)
    USBSerial_print(*(p+i), HEX);
  USBSerial_println("");

   
  bool isbusy;
  do {
    isbusy = busy();
    USBSerial_print("Is Busy?: ");
    USBSerial_println(isbusy ? "Yes" : "No");
    delay(1000);
  } while (isbusy);

  // Read status
  USBSerial_print("Status: ");
  uint8_t status = readStatus();
  USBSerial_println(status);

}

void loop() { 
  // not used
}
