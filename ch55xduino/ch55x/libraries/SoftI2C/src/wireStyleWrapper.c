#include "SoftI2C.h"
#include <Arduino.h>

/**************************** Wire I2C interface from Adafruit */

void Wire_begin(uint8_t scl, __xdata uint8_t sda) {
  scl_pin = scl; //extern variable in SoftI2C.h
  sda_pin = sda;
  I2CInit();
}

bool Wire_scan(uint8_t i2caddr) {
  return Wire_writeBytes(i2caddr, NULL, 0);
}

bool Wire_readRegister(uint8_t i2caddr, __xdata uint8_t regaddr, uint8_t* __xdata data, __xdata uint8_t bytes) {
  // some devices require a restart rather than a write and a read
  I2CStart();
  if (I2CSend(i2caddr << 1 | 0)) {
    goto readRegError;
  }
  if (I2CSend(regaddr)) {
    goto readRegError;
  }
  I2CRestart();
  if (I2CSend(i2caddr << 1 | 1)) {
    goto readRegError;
  }
  for (uint8_t i = 0; i < bytes; i++) {
    data[i] = I2CRead();
    if (i == bytes - 1) {
      I2CNak();  // NAK on last byte
    } else {
      I2CAck();  // ACK on other bytes
    }
  }
  I2CStop();
  return true;
readRegError:
  I2CStop();
  return false;
}

bool Wire_readRegister16bitAddr(uint8_t i2caddr, __xdata uint16_t regaddr, uint8_t* __xdata data, __xdata uint8_t bytes) {
  // some devices require a restart rather than a write and a read
  I2CStart();
  if (I2CSend(i2caddr << 1 | 0)) {
    goto read16bitRegError;
  }
  if (I2CSend(regaddr >> 8)) {
    goto read16bitRegError;
  }
  if (I2CSend(regaddr & 0xFF)) {
    goto read16bitRegError;
  }
  I2CRestart();
  if (I2CSend(i2caddr << 1 | 1)) {
    goto read16bitRegError;
  }
  for (uint8_t i = 0; i < bytes; i++) {
    data[i] = I2CRead();
    if (i == bytes - 1) {
      I2CNak();  // NAK on last byte
    } else {
      I2CAck();  // ACK on other bytes
    }
  }
  I2CStop();
  return true;
read16bitRegError:
  I2CStop();
  return false;
}


bool Wire_writeBytes(uint8_t i2caddr, uint8_t* __xdata data, __xdata uint8_t bytes) {
  uint8_t ack_bit;

  I2CStart();
  ack_bit = I2CSend(i2caddr << 1 | 0); // Shift address and append write bit
  if (ack_bit != 0) {
    I2CStop();
    return false;
  }

  for (uint8_t i = 0; i < bytes; i++) {
    if (I2CSend(data[i]) != 0) {
      I2CStop();
      return false;
    }
  }
  I2CStop();
  return true;
}

bool Wire_readBytes(uint8_t i2caddr, uint8_t* __xdata data, __xdata uint8_t bytes) {
  uint8_t ack_bit;

  I2CStart();
  ack_bit = I2CSend(i2caddr << 1 | 1); // Shift address and append read bit
  if (ack_bit != 0) {
    I2CStop();
    return false;
  }

  for (uint8_t i = 0; i < bytes; i++) {
    data[i] = I2CRead();
    if (i == bytes - 1) {
        I2CNak();  // NAK on last byte
    } else {
        I2CAck();  // ACK on other bytes
    }
  }

  I2CStop();
  return true;
}