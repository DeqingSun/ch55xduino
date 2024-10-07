#include "tinySpiFunc.h"
#include "USBhandler.h"
#include "debugWireFunc.h"
#include "hardwareShare.h"
// refering to EP0Buffer, SetupLen, SetupReq

volatile __xdata uint8_t ledState;

volatile __data uint8_t sck_period = 10; // SCK period in microseconds (1..250)
volatile __data uint8_t poll1;           // first poll byte for write
volatile __data uint8_t poll2;           // second poll byte for write
volatile __xdata uint16_t address;       // read/write address
volatile __xdata uint16_t timeout;       // write timeout in usec
volatile __data uint8_t cmd0;            // current read/write command byte
volatile __xdata uint8_t cmd[4];         // SPI command buffer
volatile __xdata uint8_t res[4];         // SPI result buffer

extern __xdata uint8_t debugWireAutoQuit;

// ----------------------------------------------------------------------
// Issue one SPI command.
// ----------------------------------------------------------------------
static void spi(uint8_t *cmd, uint8_t *res) {
  __data uint8_t i;
  for (i = 0; i < 4; i++) {
    SPI0_DATA = *cmd++;
    while (S0_FREE == 0)
      ;
    *res++ = SPI0_DATA;
  }
}
// ----------------------------------------------------------------------
// Create and issue a read or write SPI command.
// ----------------------------------------------------------------------
static void spi_rw(void) {
  __data uint16_t a;

  a = address++;
  if (cmd0 & 0x80) { // eeprom
    a <<= 1;
  }
  cmd[0] = cmd0;
  if (a & 1) {
    cmd[0] |= 0x08;
  }
  cmd[1] = a >> 9;
  cmd[2] = a >> 1;
  spi(cmd, res);
}

uint16_t tinySpiSetupHandler() {
  __data uint8_t i;
  __data uint16_t returnLen = 0;
  switch (SetupReq) {
  case USBTINY_ECHO: // not fully tested yet...
    Ep0Buffer[1] = 0x21;
    returnLen = 8;
    break;
  case USBTINY_READ: // not implemented yet, not necessary?
    Ep0Buffer[0] = 0x0;
    returnLen = 1;
    break;
  case USBTINY_WRITE: // not implemented yet, not necessary?
  case USBTINY_CLR:
  case USBTINY_SET:
    returnLen = 0;
    break;

    // Programming requests
  case USBTINY_POWERUP: // AVRdude: usb_control(pgm, USBTINY_POWERUP,
                        // PDATA(pgm)->sck_period, RESET_LOW)
    sck_period = Ep0Buffer[2];
    if (sck_period < 11) {
      SPI0_CK_SE = 24 * sck_period;
    } else {
      SPI0_CK_SE = 255;
    }
    // we turn ON onboard LED
    ledState |= LED_FLG_SPI;
    SET_LED_WITH_MASK();

    if (Ep0Buffer[4]) {
      // not anything meaningful?
    }
    // INIT SPI BUS
    P1_DIR_PU |= ((1 << 5) | (1 << 7));
    SPI0_CTRL = bS0_MOSI_OE | bS0_SCK_OE; // mode 0
    P1_DIR_PU |= ((1 << 1));
    P1_1 = 0; // Put reset pin to output low
    returnLen = 0;
    if (debugWireAutoQuit !=
        DWAQ_NOSIGNAL) { // some verion of AVRdude will do power up at the end
      debugWireAutoQuit = DWAQ_NORMAL;
    }

    break;

  case USBTINY_POWERDOWN:
    // we turn OFF onboard LED
    ledState &= ~LED_FLG_SPI;
    SET_LED_WITH_MASK();
    // set all the pins to Hi-Z
    // SPI
    P1_DIR_PU &= ~((1 << 5) | (1 << 6) | (1 << 7));
    SPI0_CTRL = 0;
    // RESET
    P1_DIR_PU &= ~((1 << 1));
    P1_1 = 1;

    if (debugWireAutoQuit == DWAQ_NOSIGNAL) {
      debugWireAutoQuit = DWAQ_NOSIGNAL_ISP_QUIT; // do automatic quit dw
    }

    returnLen = 0;
    break;

  case USBTINY_SPI:
    returnLen =
        *((uint16_t *)(Ep0Buffer +
                       2)); // use returnLen to store first 2 byte temporarily
    spi(Ep0Buffer + 2, Ep0Buffer + 0);
    if (returnLen == 0x53AC) { // Programming Enable
      if (Ep0Buffer[2] != 0x53) {
        debugWireAutoQuit = DWAQ_NOSIGNAL;
      }
    }
    returnLen = 4;
    break;

  case USBTINY_POLL_BYTES: // not fully tested yet...
    spi(Ep0Buffer + 2, Ep0Buffer + 0);
    poll1 = Ep0Buffer[2];
    poll2 = Ep0Buffer[3];
    returnLen = 0;
    break;

  case USBTINY_FLASH_READ:
  case USBTINY_EEPROM_READ:
    address = *(uint16_t *)&Ep0Buffer[4];
    if (SetupReq == USBTINY_FLASH_READ) {
      cmd0 = 0x20;
    } else {
      cmd0 = 0xa0;
    }
    returnLen = SetupLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE
                                               : SetupLen; // 本次传输长度
    for (i = 0; i < returnLen; i++) {
      spi_rw();
      Ep0Buffer[i] = res[3];
    }
    SetupLen -= returnLen;
    break;

  case USBTINY_FLASH_WRITE:
  case USBTINY_EEPROM_WRITE:
    address = *(uint16_t *)&Ep0Buffer[4];
    timeout = *(uint16_t *)&Ep0Buffer[2];
    if (SetupReq == USBTINY_FLASH_WRITE) {
      cmd0 = 0x40;
    } else {
      cmd0 = 0xc0;
    }
    returnLen = SetupLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : SetupLen;
    break;
  }
  return returnLen;
}

void tinySpiInHandler() {
  __data uint8_t i;
  __data uint16_t returnLen = SetupLen >= DEFAULT_ENDP0_SIZE
                                  ? DEFAULT_ENDP0_SIZE
                                  : SetupLen; // 本次传输长度
  for (i = 0; i < returnLen; i++) {
    spi_rw();
    Ep0Buffer[i] = res[3];
  }
  SetupLen -= returnLen;
  UEP0_T_LEN = returnLen;
  UEP0_CTRL ^= bUEP_T_TOG;
}

void tinySpiOutHandler() {
  __data uint16_t usec;
  __data uint8_t i;
  __data uint8_t r;
  __data uint16_t returnLen = SetupLen >= DEFAULT_ENDP0_SIZE
                                  ? DEFAULT_ENDP0_SIZE
                                  : SetupLen; // 本次传输长度
  for (i = 0; i < returnLen; i++) {
    cmd[3] = Ep0Buffer[i];
    spi_rw();
    cmd[0] ^= 0x60; // turn write into read
    //
    for (usec = 0; usec < timeout;
         usec +=
         32 * sck_period) { // when timeout > 0, poll until byte is written
      spi(cmd, res);
      r = res[3];
      if (r == cmd[3] && r != poll1 && r != poll2) {
        break;
      }
    }
  }
  SetupLen -= returnLen;
  UEP0_T_LEN = (SetupLen > 0) ? returnLen : 0;
  UEP0_CTRL ^= bUEP_R_TOG;
}
