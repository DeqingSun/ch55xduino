#ifndef __USB_MAL_H
#define __USB_MAL_H

#include <stdint.h>

#define MAL_MAX_LUN 0

#define MAL_OK 0
#define MAL_FAIL 1

extern __code const uint8_t emuDisk_Inquiry_Data[];

// make sure they match
extern __code const uint8_t diskCapacity[8];
extern __code const uint8_t formatCapacity[8];
#define MASS_BLOCK_COUNT 0x4000
#define MASS_BLOCK_SIZE 512

typedef void (*pFileCBFn)(uint16_t);

union FileCBPtrUnion {
  pFileCBFn funcPtr;
  __code uint8_t *constPtr;
};
enum FileCBPtrType { CONST_DATA_FILE, FUNCTION_CALLBACK_FILE };

typedef struct _File_Entry {
  uint8_t filename[11];
  uint8_t filetime[2];
  uint8_t filedate[2];
  uint16_t filesize;
  uint8_t fileCallBackType;
  union FileCBPtrUnion fileReadHandler;
  pFileCBFn fileWriteHandler;
} File_Entry;

// Time format (16Bits):
//  Bits15~11 Hour, ranging from 0~23
//  Bits10~5 Minute, ranging from 0~59
//  Bits4~0 Second, ranging from 0~29. Each unit is 2 seconds.

// Date format (16Bits):
//  Bits15~9 Year, ranging from 0~127. Represent difference from 1980.
//  Bits8~5 Month, ranging from 1~12
//  Bits4~0 Day, ranging from 1~31

// Get high byte of time format in 16bits format
#define TIME_HB(H, M, S) (((((H) << 3)) | ((M) >> 3)))
// Get low byte of time format in 16bits format
#define TIME_LB(H, M, S) (((0)) | ((M) << 5) | (S))

// Get high byte of date format in 16bits format
#define DATE_HB(Y, M, D) (((((Y)-1980) << 1) | ((M) >> 3)))
// Get low byte of date format in 16bits format
#define DATE_LB(Y, M, D) ((0) | ((M) << 5) | (D))

uint8_t LUN_GetStatus();
void LUN_Eject();
// Read BULK_MAX_PACKET_SIZE bytes of data from device to BOT_Tx_Buf
void LUN_Read(uint32_t curAddr);
// Write BULK_MAX_PACKET_SIZE bytes of data from BOT_Rx_Buf to device
void LUN_Write(uint32_t curAddr);

#endif
