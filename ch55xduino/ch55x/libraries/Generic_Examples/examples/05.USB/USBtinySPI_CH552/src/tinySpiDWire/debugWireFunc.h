#ifndef __DEBUG_WIRE_FUNC_H__
#define __DEBUG_WIRE_FUNC_H__

#define DWIO_NONE 0
#define DWIO_SEND_BREAK (1 << 0)
#define DWIO_SET_TIMING (1 << 1)
#define DWIO_SEND_BYTES (1 << 2)
#define DWIO_WAIT_FOR_BIT (1 << 3)
#define DWIO_READ_BYTES (1 << 4)
#define DWIO_READ_PULSE_WIDTH (1 << 5)
#define DWIO_FINISHED (1 << 7)

#define DWAQ_NORMAL 0
#define DWAQ_NOSIGNAL 1
#define DWAQ_NOSIGNAL_ISP_QUIT 2
#define DWAQ_CAPTURING_WIDTH 3
#define DWAQ_SENDING_QUIT 4

void dwCaptureWidths();
void dwSendBytesBlocking();
void dwReadBytesBlocking();

void dwWaitForBitInterruptInit();
void dwSendBytesInterruptInit();
void dwReadBytesInterruptInit();

void dwSendBytesInterrupt();
void dwSendReadBytesInterrupt();
void dwReadBytesInterrupt();
void dwWaitForBitInterrupt();
void dwSendBytesWaitInterrupt();

//void timer2IntrHandler();
uint16_t debugWireSetupHandler();
void debugWireInHandler();
void debugWireOutHandler();

void debugWireProcess();

#endif
