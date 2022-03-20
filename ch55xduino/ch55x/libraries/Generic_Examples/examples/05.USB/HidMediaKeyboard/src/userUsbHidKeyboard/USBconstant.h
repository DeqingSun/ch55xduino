#ifndef __USB_CONST_DATA_H__
#define __USB_CONST_DATA_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#define  EP0_ADDR 0
#define  EP1_ADDR 10

extern __code uint8_t DevDesc[];
extern __code uint8_t CfgDesc[];
extern __code uint8_t LangDes[];
extern __code uint8_t ReportDesc[];
extern __code uint8_t SerDes[];
extern __code uint8_t Prod_Des[];
extern __code uint8_t Manuf_Des[];

extern __code uint16_t DevDescLen;
extern __code uint16_t CfgDescLen;
extern __code uint16_t LangDesLen;
extern __code uint16_t ReportDescLen;
extern __code uint16_t SerDesLen;
extern __code uint16_t Prod_DesLen;
extern __code uint16_t Manuf_DesLen;

#endif
