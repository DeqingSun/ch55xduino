#ifndef __USB_HANDLER_H__
#define __USB_HANDLER_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
// clang-format on

// clang-format off
extern __xdata __at (EP0_ADDR) uint8_t Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[];
extern __xdata __at (EP3_ADDR) uint8_t Ep3Buffer[];
// clang-format on

extern uint16_t SetupLen;
extern uint8_t SetupReq, UsbConfig;
extern const __code uint8_t *pDescr;

extern volatile uint8_t usbMsgFlags; // uint8_t usbMsgFlags copied from VUSB
#define USB_FLG_USE_USER_RW (1 << 0)
#define USB_FLG_DW_IN (1 << 1)
#define USB_FLG_DW_OUT (1 << 2)

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

// Out
#define EP0_OUT_Callback USB_EP0_OUT
#define EP1_OUT_Callback NOP_Process
#define EP2_OUT_Callback NOP_Process
#define EP3_OUT_Callback USB_EP3_OUT
#define EP4_OUT_Callback NOP_Process

// SOF
#define EP0_SOF_Callback NOP_Process
#define EP1_SOF_Callback NOP_Process
#define EP2_SOF_Callback NOP_Process
#define EP3_SOF_Callback NOP_Process
#define EP4_SOF_Callback NOP_Process

// IN
#define EP0_IN_Callback USB_EP0_IN
#define EP1_IN_Callback USB_EP1_IN
#define EP2_IN_Callback USB_EP2_IN
#define EP3_IN_Callback USB_EP3_IN
#define EP4_IN_Callback NOP_Process

// SETUP
#define EP0_SETUP_Callback USB_EP0_SETUP
#define EP1_SETUP_Callback NOP_Process
#define EP2_SETUP_Callback NOP_Process
#define EP3_SETUP_Callback NOP_Process
#define EP4_SETUP_Callback NOP_Process

#ifdef __cplusplus
extern "C" {
#endif

void USBInterrupt(void);
void USBDeviceCfg();
void USBDeviceIntCfg();
void USBDeviceEndPointCfg();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
