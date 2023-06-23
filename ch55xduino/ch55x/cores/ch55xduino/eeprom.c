/*
 created by Deqing Sun for use with CH55xduino
 */

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

void eeprom_write_byte(__data uint8_t addr, __xdata uint8_t val) {

#if defined(CH551) || defined(CH552)

  if (addr >= 128) {
    return;
  }

  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;        // Enter Safe mode
  GLOBAL_CFG |= bDATA_WE; // Enable DataFlash write
  SAFE_MOD = 0;           // Exit Safe mode
  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1;
  ROM_DATA_L = val;
  if (ROM_STATUS & bROM_ADDR_OK) { // Valid access Address
    ROM_CTRL = ROM_CMD_WRITE;      // Write
  }
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;         // Enter Safe mode
  GLOBAL_CFG &= ~bDATA_WE; // Disable DataFlash write
  SAFE_MOD = 0;            // Exit Safe mode

#else
  addr; // TBD
  val;
  return;
#endif
}

uint8_t eeprom_read_byte(__data uint8_t addr) {
#if defined(CH551) || defined(CH552)

  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
  ROM_ADDR_L = addr << 1; // Addr must be even
  ROM_CTRL = ROM_CMD_READ;
  return ROM_DATA_L;
#else
  addr; // TBD
  return 0;
#endif
}
