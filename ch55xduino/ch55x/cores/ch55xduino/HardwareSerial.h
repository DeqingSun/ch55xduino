#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <stdint.h>
#include <stdio.h>
#include "include/ch5xx.h"

#define SERIAL0_TX_BUFFER_SIZE 16
#define SERIAL0_RX_BUFFER_SIZE 16
#define SERIAL1_TX_BUFFER_SIZE 16
#define SERIAL1_RX_BUFFER_SIZE 16

#define UART0_FLG_SENDING     (1<<0)

uint8_t Serial0(void);
void Serial0_begin(unsigned long baud);

uint8_t Serial0_available(void);
uint8_t Serial0_read(void);
uint8_t Serial0_write(uint8_t c);
void Serial0_flush(void);

void Serial0_end(void);

void uart0IntRxHandler();
void uart0IntTxHandler();


uint8_t Serial1(void);
void Serial1_begin(unsigned long baud);

uint8_t Serial1_available(void);
uint8_t Serial1_read(void);
uint8_t Serial1_write(uint8_t c);
void Serial1_flush(void);

void Serial1_end(void);

void uart1IntRxHandler();
void uart1IntTxHandler();

#endif
