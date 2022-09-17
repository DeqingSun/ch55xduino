#include "SPIFlash.h"

// Erase, read and write operations example
//
// ATTENTION: THIS SKETCH ERASES A BLOCK OF 4K FROM CHIP, IF YOU HAVE DATA IN IT, WILL BE LOST

void waitOperation() {    
  bool isbusy;
  do {
    isbusy = busy();
    USBSerial_print("Waiting chip be ready: ");
    USBSerial_println(isbusy ? "Not yet" : "Done");
    delay(1000);
  } while (isbusy);

}

void setup() {  
  
  initialize();

  while (!USBSerial()) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  USBSerial_println("*** Accessing W25QXX flash chip via SPI. ***");


  // erase a single block of 4k
  USBSerial_println("Erase a block (4K) addr 0");
  blockErase4K(0);
  
  waitOperation();

  USBSerial_println("---");

  // Write 1 byte
  USBSerial_println("Write byte 1A at address 0");
  writeByte(0, 0x1A);

    // Read 1 byte
  USBSerial_print("Read one byte at address 0: ");
  USBSerial_println(readByte(0), HEX);


  USBSerial_println("---");

  // erase a single block of 4k
  USBSerial_println("Erase a block (4K) addr 0");
  blockErase4K(0);
  
  waitOperation();

  // Read 1 byte
  USBSerial_print("Read one byte at address 0: ");
  USBSerial_println(readByte(0), HEX);

  USBSerial_println("---");

  // Write bytes
  USBSerial_println("Write 'hello, world!' at address 0");
  const char msg[14]  = "hello, world!";
  writeBytes(0, msg, 14);

  // Read bytes
  USBSerial_print("Read bytes at address 0: ");
  char input[14];
  readBytes(0, input, 14);
  USBSerial_println(input);
    
}

void loop() 
{
  // not used
}