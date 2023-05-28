#ifndef __CONST_DATA_H__
#define __CONST_DATA_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdUSBDescriptors.h"

#define  SET_LINE_CODING                0X20            // Configures DTE rate, stop-bits, parity, and number-of-character
#define  GET_LINE_CODING                0X21            // This request allows the host to find out the currently configured line coding.
#define  SET_CONTROL_LINE_STATE         0X22            // This request generates RS-232/V.24 style control signals.


extern __code USB_Descriptor_Device_t DevDesc;
extern __code uint8_t CfgDesc[];
extern __code uint8_t LangDes[];
extern __code uint16_t SerDes[];
extern __code uint16_t Prod_Des[];
extern __code uint16_t CDC_Des[];
extern __code uint16_t Manuf_Des[];



extern __code uint16_t CfgDescLen;

#endif
