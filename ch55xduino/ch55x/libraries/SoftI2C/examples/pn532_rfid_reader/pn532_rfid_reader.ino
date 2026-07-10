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
    USBSerial_println();

    if (uidLength == 4) {
      // We probably have a Mifare Classic card ...
      USBSerial_println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      USBSerial_println("Trying to authenticate block 4 with default KEYA value");
      __xdata uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = pn532_mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success) {
        USBSerial_println("Sector 1 (Blocks 4..7) has been authenticated");
        __xdata uint8_t data[16];

        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        // {
        //   data[0] = 0x01;
        //   data[1] = 0x02;
        //   data[2] = 0x03;
        //   data[3] = 0x04;
        //   data[4] = 0x05;
        //   data[5] = 0x06;
        //   data[6] = 0x07;
        //   data[7] = 0x08;
        //   data[8] = 0x09;
        //   data[9] = 0x0A;
        //   data[10] = 0x0B;
        //   data[11] = 0x0C;
        //   data[12] = 0x0D;
        //   data[13] = 0x0E;
        //   data[14] = 0x0F;
        //   data[15] = 0x10;
        //   pn532_mifareclassic_WriteDataBlock(4, data);
        // }

        // Try to read the contents of block 4
        success = pn532_mifareclassic_ReadDataBlock(4, data);

        if (success) {
          // Data seems to have been read ... spit it out
          USBSerial_println("Reading Block 4:");
          pn532_PrintHex(data, 16);
          USBSerial_println();

          // Wait a bit before reading the card again
          delay(1000);
        } else {
          USBSerial_println("Ooops ... unable to read the requested block.  Try another key?");
        }
      } else {
        USBSerial_println("Ooops ... authentication failed: Try another key?");
      }
    }

    if (uidLength == 7) {
      // We probably have a Mifare Ultralight card ...
      USBSerial_println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // If you want to write something to block 4 to test with, uncomment
      // the following line and this text should be read back in a minute
      // {
      //   __xdata uint8_t data[4] = { 0x01, 0x02, 0x03, 0x04};
      //   pn532_mifareultralight_WritePage(4, data);
      // }

      // Try to read the first general-purpose user page (#4)
      USBSerial_println("Reading page 4");
      __xdata uint8_t data[32];
      success = pn532_mifareultralight_ReadPage(4, data);
      if (success) {
        // Data seems to have been read ... spit it out
        pn532_PrintHex(data, 4);
        USBSerial_println("");

        // Wait a bit before reading the card again
        delay(1000);
      } else {
        USBSerial_println("Ooops ... unable to read the requested page!?");
      }
    }
  }
  USBSerial_flush();
  delay(1000);
}
