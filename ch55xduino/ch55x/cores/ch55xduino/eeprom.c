/*
 created by Deqing Sun for use with CH55xduino
 */

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

void eeprom_write_byte_2_params_DPTR (uint16_t addr_val){
//using a single parameter of 16bit number utilize both DPL and DPH, avoid using memory to pass parameters
#define ADDR_PARAM ((addr_val>>8)&0xff)
#define VAL_PARAM ((addr_val>>0)&0xff)
    
#if defined(CH551) || defined(CH552)
    
    if (ADDR_PARAM>=128){
        return;
    }
    
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;                                                           //Enter Safe mode
    GLOBAL_CFG |= bDATA_WE;                                                    //Enable DataFlash write
    SAFE_MOD = 0;                                                              //Exit Safe mode
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = ADDR_PARAM<<1;
    ROM_DATA_L = VAL_PARAM;
    if ( ROM_STATUS & bROM_ADDR_OK ) {                                          // Valid access Address
        ROM_CTRL = ROM_CMD_WRITE;                                               // Write
    }
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;                                                           //Enter Safe mode
    GLOBAL_CFG &= ~bDATA_WE;                                                   //Disable DataFlash write
    SAFE_MOD = 0;                                                              //Exit Safe mode
    
#else
    addr_val;   //TBD
    return;
#endif
}

uint8_t eeprom_read_byte (uint8_t addr){
#if defined(CH551) || defined(CH552)
    
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    ROM_ADDR_L = addr<<1;                                                       //Addr must be even
    ROM_CTRL = ROM_CMD_READ;
    return ROM_DATA_L;
#else
    addr;   //TBD
    return 0;
#endif
}
