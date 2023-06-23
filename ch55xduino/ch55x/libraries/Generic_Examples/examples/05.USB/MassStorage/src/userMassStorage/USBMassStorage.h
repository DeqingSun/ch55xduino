#ifndef __USB_MASS_STORAGE_H__
#define __USB_MASS_STORAGE_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBMediumAccess.h"
// clang-format on

#ifdef __cplusplus
extern "C" {
#endif

void USBInit(void);

#ifdef __cplusplus
} // extern "C"
#endif

// some needed marco

// uint16_t
#define U16B0(u) (((uint8_t *)&u)[0])
#define U16B1(u) (((uint8_t *)&u)[1])
// uint32_t
#define U32B0(u) (((uint8_t *)&u)[0])
#define U32B1(u) (((uint8_t *)&u)[1])
#define U32B2(u) (((uint8_t *)&u)[2])
#define U32B3(u) (((uint8_t *)&u)[3])

#define U16_XBig(dstAddr, srcAddr)                                             \
  {                                                                            \
    ((uint8_t *)dstAddr)[0] = ((uint8_t *)srcAddr)[1];                         \
    ((uint8_t *)dstAddr)[1] = ((uint8_t *)srcAddr)[0];                         \
  }

#define U16_XLittle(dstAddr, srcAddr)                                          \
  {                                                                            \
    ((uint8_t *)dstAddr)[0] = ((uint8_t *)srcAddr)[0];                         \
    ((uint8_t *)dstAddr)[1] = ((uint8_t *)srcAddr)[1];                         \
  }

#define U32_XBig(dstAddr, srcAddr)                                             \
  {                                                                            \
    ((uint8_t *)dstAddr)[0] = ((uint8_t *)srcAddr)[3];                         \
    ((uint8_t *)dstAddr)[1] = ((uint8_t *)srcAddr)[2];                         \
    ((uint8_t *)dstAddr)[2] = ((uint8_t *)srcAddr)[1];                         \
    ((uint8_t *)dstAddr)[3] = ((uint8_t *)srcAddr)[0];                         \
  }

#define U32_XLittle(dstAddr, srcAddr)                                          \
  {                                                                            \
    ((uint8_t *)dstAddr)[0] = ((uint8_t *)srcAddr)[0];                         \
    ((uint8_t *)dstAddr)[1] = ((uint8_t *)srcAddr)[1];                         \
    ((uint8_t *)dstAddr)[2] = ((uint8_t *)srcAddr)[2];                         \
    ((uint8_t *)dstAddr)[3] = ((uint8_t *)srcAddr)[3];                         \
  }

/* Includes ------------------------------------------------------------------*/
extern __xdata uint8_t Ep1Buffer[];
#define BOT_Rx_Buf (Ep1Buffer)
#define BOT_Tx_Buf (Ep1Buffer + 64)
#define BULK_MAX_PACKET_SIZE 64
#define BOT_EP_Tx_RTOG()                                                       \
  { UEP1_CTRL &= ~bUEP_T_TOG; }
#define BOT_EP_Rx_RTOG()                                                       \
  { UEP1_CTRL &= ~bUEP_R_TOG; }
#define BOT_EP_Tx_Valid()                                                      \
  { UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; }
#define BOT_EP_Rx_Valid()                                                      \
  { UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK; }
#define BOT_EP_Tx_Stall()                                                      \
  { UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; }
#define BOT_EP_Rx_Stall()                                                      \
  { UEP1_CTRL = UEP1_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; }
#define BOT_EP_Tx_Count(val)                                                   \
  { UEP1_T_LEN = val; }
#define BOT_EP_Rx_Length USB_RX_LEN

/* Exported types ------------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */

typedef struct _Bulk_Only_CBW {
  uint8_t dSignature0;
  uint8_t dSignature1;
  uint8_t dSignature2;
  uint8_t dSignature3;

  uint8_t dTag0;
  uint8_t dTag1;
  uint8_t dTag2;
  uint8_t dTag3;

  uint32_t dDataLength;
  uint8_t bmFlags;
  uint8_t bLUN;
  uint8_t bCBLength;
  uint8_t CB[16];
} Bulk_Only_CBW;

/* Exported constants --------------------------------------------------------*/

/*****************************************************************************/
/*********************** Bulk-Only Transfer State machine ********************/
/*****************************************************************************/
#define BOT_IDLE 0              // Idle state
#define BOT_DATA_OUT 1          // Data Out state
#define BOT_DATA_IN 2           // Data In state
#define BOT_DATA_IN_LAST 3      // Last Data In Last
#define BOT_CSW_Send 4          // Command Status Wrapper
#define BOT_ERROR 5             // Error state
#define BOT_DATA_IN_LAST_FAIL 6 // Send a Check_Status to host

// BOT CBW & CSW Signatures ("USBC")
#define BOT_CBW_PACKET_LENGTH 31
#define BOT_CBW_SIGNATURE0 0x55
#define BOT_CBW_SIGNATURE1 0x53
#define BOT_CBW_SIGNATURE2 0x42
#define BOT_CBW_SIGNATURE3 0x43

//("USBS")
#define CSW_DATA_LENGTH 13
#define BOT_CSW_SIGNATURE0 0x55
#define BOT_CSW_SIGNATURE1 0x53
#define BOT_CSW_SIGNATURE2 0x42
#define BOT_CSW_SIGNATURE3 0x53

/* CSW Status Definitions */
#define CSW_CMD_PASSED 0x00
#define CSW_CMD_FAILED 0x01
#define CSW_PHASE_ERROR 0x02

#define SEND_CSW_DISABLE 0
#define SEND_CSW_ENABLE 1

#define DIR_IN 0
#define DIR_OUT 1
#define BOTH_DIR 2

void Mass_Storage_In(void);
void Mass_Storage_Out(void);
void CBW_Decode(void);
void Transfer_Data_Request_MergeParameter(uint32_t Data_Len_Data_Pointer);
#define Transfer_Data_Request(DATA_PTR, DATA_LEN)                              \
  {                                                                            \
    Transfer_Data_Request_MergeParameter(((((uint32_t)(DATA_LEN)) << 16)) |    \
                                         ((uint16_t)(DATA_PTR)));              \
  }
void Reply_Request(uint8_t Data_Len);
void Transfer_Failed_ReadWrite(void);
void Set_CSW_MergeParameter(uint16_t Send_Permission_CSW_Status);
#define Set_CSW(CSW_Status, Send_Permission)                                   \
  { Set_CSW_MergeParameter(((Send_Permission) << 8) | (CSW_Status)); }
void Bot_Abort(uint8_t Direction);

void SCSI_Read10_Cmd();
void SCSI_Write10_Cmd();
uint8_t SCSI_Address_Management(uint8_t Cmd);

#endif
