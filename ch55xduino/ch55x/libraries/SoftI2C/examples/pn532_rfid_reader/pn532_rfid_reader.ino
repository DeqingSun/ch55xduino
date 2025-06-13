#include <SoftI2C.h>
#include "pn532_lib.h"

// if you are using a red square PN532 board with 2 surface mount switches in one package
// you can set I2C inteface by
// move switch 1 to ON side
// move switch 2 to OFF (numbers) side

void setup() {
  //pass SCL and SDA
  Wire_begin(30, 31);

  delay(400);

  while (!USBSerial()) {
    // wait for USB serial to be ready
    delay(100);
  }

  if (!Wire_scan(PN532_I2C_ADDRESS)) {
    while (1) {
      USBSerial_print("PN532 not found on I2C address 0x");
      USBSerial_println(PN532_I2C_ADDRESS, HEX);
      USBSerial_println("Please check your wiring and try again.");
      USBSerial_println("If you are using a red square PN532 board, make sure the I2C switch is set correctly.");
      USBSerial_flush();
      delay(1000);
    }
  } else {
    USBSerial_print("PN532 found! on I2C address 0x");
    USBSerial_println(PN532_I2C_ADDRESS, HEX);
    USBSerial_flush();
  }

  uint32_t firmwareVersion = pn532_getFirmwareVersion();
  if (firmwareVersion == 0) {
    while (1) {
      USBSerial_println("Firmware version not found. Please check your wiring.");
      USBSerial_flush();
      delay(1000);
    }
  } else {
    USBSerial_print("Found chip PN5");
    USBSerial_print((firmwareVersion >> 24) & 0xFF, HEX);
    USBSerial_print(" Firmware ver. ");
    USBSerial_print((firmwareVersion >> 16) & 0xFF, DEC);
    USBSerial_print((char)'.');
    USBSerial_print((firmwareVersion >> 8) & 0xFF, DEC);
    USBSerial_print(" Firmware version: ");
    USBSerial_println(firmwareVersion, HEX);
    USBSerial_flush();
  }

  pn532_SAMConfig();

  USBSerial_print("Waiting for an ISO14443A Card ...");
  USBSerial_flush();
}


void loop() {

  __xdata uint8_t success;
  __xdata uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  __xdata uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = pn532_readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 0);

  if (success) {
    // Display some basic information about the card
    USBSerial_println("Found an ISO14443A card");
    USBSerial_print("  UID Length: ");
    USBSerial_print(uidLength, DEC);
    USBSerial_println(" bytes");
    USBSerial_print("  UID Value: ");
    pn532_PrintHex(uid, uidLength);
  }

  delay(100);
}
