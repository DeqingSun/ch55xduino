#include "pn532_lib.h"

#define PN532_PACKBUFFSIZ 64
__xdata uint8_t pn532_packetbuffer[PN532_PACKBUFFSIZ];
//__xdata uint8_t pn532_swapbuffer[PN532_PACKBUFFSIZ];
__code uint8_t pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
__code uint8_t pn532response_firmwarevers[] = {0x00, 0x00, 0xFF,
                                               0x06, 0xFA, 0xD5};

uint8_t I2CSendAndWaitForStretch(uint8_t i2cData) {
  uint8_t i, ack_bit;
  for (i = 0; i < 8; i++) {
    if ((i2cData & 0x80) == 0)
      digitalWrite(sda_pin, LOW);
    else
      digitalWrite(sda_pin, HIGH);

    if (i == 0) {
      delayMicroseconds(3);
      pinMode(scl_pin, INPUT_PULLUP);
      digitalWrite(scl_pin, HIGH);
      while (digitalRead(scl_pin) == LOW) {
        // wait for clock stretch
      }
      pinMode(scl_pin, OUTPUT);
    }
    digitalWrite(scl_pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(scl_pin, LOW);
    i2cData <<= 1;
  }
  pinMode(sda_pin, INPUT_PULLUP);
  digitalWrite(sda_pin, HIGH);
  digitalWrite(scl_pin, HIGH);
  ack_bit = digitalRead(sda_pin);
  pinMode(sda_pin, OUTPUT_OD);
  digitalWrite(scl_pin, LOW);
  return ack_bit;
}

void pn532_wiresendcommand(uint8_t cmdlen) {
  uint8_t checksum;

  cmdlen++;

  delay(2); // or whatever the delay is for waking up the board

  int8_t ack_bit;

  I2CStart();
  ack_bit = I2CSendAndWaitForStretch(PN532_I2C_ADDRESS << 1 |
                                     0); // Shift address and append write bit
  if (ack_bit != 0) {
    I2CStop();
    return;
  }

  checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;

  I2CSendAndWaitForStretch(PN532_PREAMBLE);
  I2CSendAndWaitForStretch(PN532_PREAMBLE);
  I2CSendAndWaitForStretch(PN532_STARTCODE2);

  I2CSendAndWaitForStretch(cmdlen);
  I2CSendAndWaitForStretch(~cmdlen + 1);

  I2CSendAndWaitForStretch(PN532_HOSTTOPN532);
  checksum += PN532_HOSTTOPN532;

  for (uint8_t i = 0; i < cmdlen - 1; i++) {
    I2CSendAndWaitForStretch(pn532_packetbuffer[i]);
    checksum += pn532_packetbuffer[i];
  }

  I2CSendAndWaitForStretch(~checksum);
  I2CSendAndWaitForStretch(PN532_POSTAMBLE);

  I2CStop();
}

void pn532_wirereaddata(uint8_t n) {
  uint16_t timer = 0;

  delay(2);

  uint8_t ack_bit;

  I2CStart();
  ack_bit =
      I2CSend(PN532_I2C_ADDRESS << 1 | 1); // Shift address and append read bit
  if (ack_bit != 0) {
    I2CStop();
    return;
  }

  I2CRead(); // Read the leading 0x01 byte
  I2CAck();  // ACK for the leading byte

  for (uint8_t i = 0; i < n; i++) {
    pn532_packetbuffer[i] = I2CRead();
    if (i == n - 1) {
      I2CNak(); // NAK on last byte
    } else {
      I2CAck(); // ACK on other bytes
    }
  }

  I2CStop();
}

uint8_t pn532_isReady() {
  uint8_t status = 0;
  I2CStart();
  I2CSendAndWaitForStretch((PN532_I2C_ADDRESS << 1) | 1); // Read mode
  status = I2CRead(); // Implement this to read a byte and send NACK
  I2CNak();
  I2CStop();
  return (status); // LSB==1 means ready
}

uint8_t pn532_readackframe(void) {
  pn532_wirereaddata(6); // Read 6 bytes
  uint8_t contentSame = 1;
  for (uint8_t i = 0; i < 6; i++) {
    if (pn532_packetbuffer[i] != pn532ack[i]) {
      contentSame = 0; // Content does not match
      break;
    }
  }
  return contentSame; // Return 1 if content matches, 0 otherwise
}

uint8_t pn532_sendCommandCheckAck(uint8_t cmdlen, __xdata uint16_t timeout) {
  __xdata uint16_t timer = 0;

  // write the command
  pn532_wiresendcommand(cmdlen);
  delayMicroseconds(100);
  while (!pn532_isReady()) {
    if (timeout != 0) {
      timer += 10;
      if (timer > timeout)
        return false; // Timeout
    }
    delay(10);
  }
  if (!pn532_readackframe()) {
    return false;
  }
  return true; // ack'd command
}

uint32_t pn532_getFirmwareVersion(void) {
  uint32_t response;

  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

  if (!pn532_sendCommandCheckAck(1, 1000))
    return 0;

  // read data packet
  pn532_wirereaddata(12);

  uint8_t first6bytesSame = 1;
  for (uint8_t i = 0; i < 6; i++) {
    if (pn532_packetbuffer[i] != pn532response_firmwarevers[i]) {
      first6bytesSame = 0; // Content does not match
      break;
    }
  }
  if (!first6bytesSame) {
    return 0; // Firmware version response does not match expected
  }

  response = pn532_packetbuffer[7];
  response <<= 8;
  response |= pn532_packetbuffer[8];
  response <<= 8;
  response |= pn532_packetbuffer[9];
  response <<= 8;
  response |= pn532_packetbuffer[10];

  return response;
}

uint8_t pn532_SAMConfig(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // Normal mode
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // Use IRQ pin

  if (!pn532_sendCommandCheckAck(4, 1000))
    return false;

  // read data packet
  pn532_wirereaddata(9);

  return (pn532_packetbuffer[6] == 0x15);
}

uint8_t pn532_readPassiveTargetID(uint8_t cardbaudrate,
                                  __xdata uint8_t *__xdata uid,
                                  __xdata uint8_t *__xdata uidLength,
                                  __xdata uint16_t timeout) {
  pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
  pn532_packetbuffer[1] = 1; // max 1 cards at once (we can set this to 2 later)
  pn532_packetbuffer[2] = cardbaudrate;

  if (!pn532_sendCommandCheckAck(3, timeout)) {
    return false; // no card found
  }
  // Wait for a card to enter the field
  uint8_t status = PN532_I2C_BUSY;
  uint16_t timer = 0;
  while (pn532_isReady() != PN532_I2C_READY) {
    if (timeout != 0) {
      timer += 10;
      if (timer > timeout) {
        return 0x0;
      }
    }
    delay(10);
  }

  // read data packet
  pn532_wirereaddata(20);

  // check some basic stuff
  /* ISO14443A card response should be in the following format:

    byte            Description
    -------------   ------------------------------------------
    b0..6           Frame header and preamble
    b7              Tags Found
    b8              Tag Number (only one used in this example)
    b9..10          SENS_RES
    b11             SEL_RES
    b12             NFCID Length
    b13..NFCIDLen   NFCID                                      */

  if (pn532_packetbuffer[7] != 1)
    return 0;

  uint16_t sens_res = pn532_packetbuffer[9];
  sens_res <<= 8;
  sens_res |= pn532_packetbuffer[10];

  /* Card appears to be Mifare Classic */
  *uidLength = pn532_packetbuffer[12];

  for (uint8_t i = 0; i < pn532_packetbuffer[12]; i++) {
    uid[i] = pn532_packetbuffer[13 + i];
  }

  return 1;
}

void pn532_PrintHex(uint8_t *__xdata data, __xdata uint32_t numBytes) {
  uint32_t szPos;
  for (szPos = 0; szPos < numBytes; szPos++) {
    USBSerial_print("0x");
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
      USBSerial_print(("0"));
    USBSerial_print(data[szPos] & 0xff, HEX);
    if ((numBytes > 1) && (szPos != numBytes - 1)) {
      USBSerial_print(" ");
    }
  }
  USBSerial_println();
  USBSerial_flush();
}