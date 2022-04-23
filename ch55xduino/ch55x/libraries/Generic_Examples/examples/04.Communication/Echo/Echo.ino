/*
  Echo

  A simple example echoes back every line of data it receives.
  Also it prints how many lines it has echoed.

  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

*/

//This is a fairly large array, store it in external memory with keyword __xdata
__xdata char recvStr[64];
uint8_t recvStrPtr = 0;
bool stringComplete = false;
uint16_t echoCounter = 0;

void setup() {
  // put your setup code here, to run once:
}

void loop() {

  while (USBSerial_available()) {
    char serialChar = USBSerial_read();
    if ((serialChar == '\n') || (serialChar == '\r') ) {
      recvStr[recvStrPtr] = '\0';
      if (recvStrPtr > 0) {
        stringComplete = true;
        break;
      }
    } else {
      recvStr[recvStrPtr] = serialChar;
      recvStrPtr++;
      if (recvStrPtr == 63) {
        recvStr[recvStrPtr] = '\0';
        stringComplete = true;
        break;
      }
    }
  }

  if (stringComplete) {
    USBSerial_print("ECHO:");
    USBSerial_println(recvStr);
    USBSerial_flush();
    stringComplete = false;
    recvStrPtr = 0;

    echoCounter++;
    USBSerial_print("echo count: ");
    USBSerial_println(echoCounter);
    USBSerial_flush();
  }
}
