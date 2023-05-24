#include "via.h"
#include "USBhandler.h"
#include <Arduino.h>
#include "../../keyboardConfig.h"

enum {
    ID_GET_PROTOCOL_VERSION = 0x01,
    ID_GET_KEYBOARD_VALUE,
    ID_SET_KEYBOARD_VALUE,
    ID_KEYMAP_GET_KEYCODE,
    ID_KEYMAP_SET_KEYCODE,
    ID_KEYMAP_RESET,
    ID_LIGHTING_SET_VALUE,
    ID_LIGHTING_GET_VALUE,
    ID_LIGHTING_SAVE,
    ID_EEPROM_RESET,
    ID_BOOTLOADER_JUMP,
    ID_MACRO_GET_COUNT,
    ID_MACRO_GET_BUFFER_SIZE,
    ID_MACRO_GET_BUFFER,
    ID_MACRO_SET_BUFFER,
    ID_MACRO_RESET, //0x10
    ID_KEYMAP_GET_LAYER_COUNT,
    ID_KEYMAP_GET_BUFFER,
    ID_KEYMAP_SET_BUFFER,
    ID_UNHANDLED = 0xFF,
};

__xdata uint8_t viaBuffer[32];
volatile __xdata uint8_t viaCmdReceived = 0;
    
void raw_hid_send(){
    USB_EP1_send(8);
}

void raw_hid_receive(void) {
    memcpy(viaBuffer, Ep1Buffer+1, 32);
    viaCmdReceived = 1;
}

uint16_t dynamic_keymap_get_keycode(__data uint8_t layer, __xdata uint8_t row, uint8_t col) {
    __data uint8_t addr = ((layer*ROWS_COUNT*COLS_COUNT) + row*COLS_COUNT + col)*2;
    return eeprom_read_byte(addr)<<8 | eeprom_read_byte(addr+1);
}

void via_process(void) {
    if (viaCmdReceived == 0){
        return;
    }
    viaCmdReceived = 0;

    memcpy(Ep1Buffer+64+1, viaBuffer, 32);

    Serial0_println("via");
    Serial0_println(viaBuffer[0], HEX);
    Serial0_println(viaBuffer[1], HEX);
    Serial0_println(viaBuffer[2], HEX);
    
    switch( viaBuffer[0] ) {
        case ID_GET_PROTOCOL_VERSION:
            Ep1Buffer[64+1+1] = 0;
            Ep1Buffer[64+1+2] = 0;
            break;
        case ID_GET_KEYBOARD_VALUE:
            for(uint8_t i=2; i<32;i++) {
                Ep1Buffer[64+1+i]= 0;
            }
            break;
        case ID_SET_KEYBOARD_VALUE:
            break;
        case ID_KEYMAP_GET_KEYCODE:
            {
                __data uint16_t keycode = dynamic_keymap_get_keycode(viaBuffer[1], viaBuffer[2], viaBuffer[3]);
                Ep1Buffer[64+1+4] = (keycode >> 8)&0xFF;
                Ep1Buffer[64+1+5] = keycode&0xFF;
            }
            break;
        case ID_KEYMAP_SET_KEYCODE:
            {
                __data uint8_t layer = viaBuffer[1];
                __data uint8_t row   = viaBuffer[2];
                __data uint8_t col   = viaBuffer[3];
                __data uint8_t addr = ((layer*ROWS_COUNT*COLS_COUNT) + row*COLS_COUNT + col)*2;
                
                eeprom_write_byte(addr, viaBuffer[4]);
                eeprom_write_byte(addr+1, viaBuffer[5]);
            }
            break;
        case ID_MACRO_GET_COUNT:
            Ep1Buffer[64+1+1] = 0;
            break;
        case ID_MACRO_GET_BUFFER_SIZE:
            Ep1Buffer[64+1+2] = 0;
            Ep1Buffer[64+1+3] = 0;
            break;
        case ID_MACRO_GET_BUFFER:
            break;
        case ID_MACRO_SET_BUFFER:
            break;
            
        case ID_KEYMAP_GET_LAYER_COUNT: //0x11
            Ep1Buffer[64+1+1] = LAYER_COUNT;
            break;
        case ID_KEYMAP_GET_BUFFER:  //0x12
            {
                __data uint16_t offset = (viaBuffer[1] << 8) | viaBuffer[2];   //well I know it overflow
                __data uint8_t size   = viaBuffer[3]; // size <= 28
                for(uint8_t i=0;i<size;i++) {
                    Ep1Buffer[64+1+4+i] = eeprom_read_byte(offset+i);
                }
            }
            break;
        case ID_KEYMAP_SET_BUFFER:
            {
                __data uint16_t offset = (viaBuffer[1] << 8) | viaBuffer[2];   //well I know it overflow
                __data uint8_t size   = viaBuffer[3]; // size <= 28
                for(uint8_t i=0;i<size;i++) {
                    eeprom_write_byte(offset+i, viaBuffer[4+i]);
                }
            }
            break;
        default:
            Ep1Buffer[64+1+0] = ID_UNHANDLED;
            break;
    }
    //delay(2);
    raw_hid_send();
}
                
