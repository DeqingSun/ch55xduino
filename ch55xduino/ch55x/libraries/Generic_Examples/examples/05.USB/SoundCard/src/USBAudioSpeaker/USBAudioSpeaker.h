#ifndef __USB_MASS_STORAGE_H__
#define __USB_MASS_STORAGE_H__

#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

#ifdef __cplusplus
} // extern "C"
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#define mTimer2RunCTL( SS )    {TR2 = SS ? START : STOP;}

//some needed marco

// uint16_t
#define U16B0(u) (((uint8_t*)&u)[0])
#define U16B1(u) (((uint8_t*)&u)[1])
// uint32_t
#define U32B0(u) (((uint8_t*)&u)[0])
#define U32B1(u) (((uint8_t*)&u)[1])
#define U32B2(u) (((uint8_t*)&u)[2])
#define U32B3(u) (((uint8_t*)&u)[3])

#define U16_XBig(dstAddr, srcAddr) {    \
((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[1];    \
((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[0];    \
}

#define U16_XLittle(dstAddr, srcAddr) {    \
((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];    \
((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];    \
}

#define U32_XBig(dstAddr, srcAddr) {    \
((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[3];    \
((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[2];    \
((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[1];    \
((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[0];    \
}

#define U32_XLittle(dstAddr, srcAddr) {    \
((uint8_t*)dstAddr)[0] = ((uint8_t*)srcAddr)[0];    \
((uint8_t*)dstAddr)[1] = ((uint8_t*)srcAddr)[1];    \
((uint8_t*)dstAddr)[2] = ((uint8_t*)srcAddr)[2];    \
((uint8_t*)dstAddr)[3] = ((uint8_t*)srcAddr)[3];    \
}

/* Includes ------------------------------------------------------------------*/
extern __xdata uint8_t Ep1Buffer[];

#define BOT_Rx_Buf (Ep1Buffer)
#define BOT_Tx_Buf (Ep1Buffer+64)
#define BULK_MAX_PACKET_SIZE 64
#define BOT_EP_Tx_RTOG()    {UEP1_CTRL &= ~bUEP_T_TOG;}
#define BOT_EP_Rx_RTOG()    {UEP1_CTRL &= ~bUEP_R_TOG;}
#define BOT_EP_Tx_Valid()    {UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;}
#define BOT_EP_Tx_ISO_Valid()    {UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_TOUT;}
#define BOT_EP_Rx_Valid()    {UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;}
#define BOT_EP_Rx_ISO_Valid()    {UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES |UEP_R_RES_TOUT;}
#define BOT_EP_Tx_Stall()    {UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;}
#define BOT_EP_Rx_Stall()    {UEP1_CTRL = UEP1_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;}
#define BOT_EP_Tx_Count(val) {UEP1_T_LEN = val;}
#define BOT_EP_Rx_Length    USB_RX_LEN


/* Exported constants --------------------------------------------------------*/

#define DIR_IN                        0
#define DIR_OUT                       1
#define BOTH_DIR                      2

void Mass_Storage_Out (void);

#endif

