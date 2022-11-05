#ifndef __TINY_SPI_H__
#define __TINY_SPI_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

//const uint8_t LITTLE_WIRE_VERSION = 0x13;
enum
{
    // Generic requests
    USBTINY_ECHO  = 0,  // echo test
    USBTINY_READ  = 1,  // read byte (wIndex:address)
    USBTINY_WRITE = 2,  // write byte (wIndex:address, wValue:value)
    USBTINY_CLR   = 3,  // clear bit (wIndex:address, wValue:bitno)
    USBTINY_SET   = 4,  // set bit (wIndex:address, wValue:bitno)
    
    // Programming requests
    USBTINY_POWERUP      = 5,   // apply power (wValue:SCK-period, wIndex:RESET)
    USBTINY_POWERDOWN    = 6,   // remove power from chip
    USBTINY_SPI          = 7,   // issue SPI command (wValue:c1c0, wIndex:c3c2)
    USBTINY_POLL_BYTES   = 8,   // set poll bytes for write (wValue:p1p2)
    USBTINY_FLASH_READ   = 9,   // read flash (wIndex:address)
    USBTINY_FLASH_WRITE  = 10,  // write flash (wIndex:address, wValue:timeout)
    USBTINY_EEPROM_READ  = 11,  // read eeprom (wIndex:address)
    USBTINY_EEPROM_WRITE = 12,  // write eeprom (wIndex:address, wValue:timeout)
};

uint16_t tinySpiSetupHandler();
void tinySpiInHandler();
void tinySpiOutHandler();

#endif
