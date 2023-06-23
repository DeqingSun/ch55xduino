/*
 created by Deqing Sun for use with CH55xduino
 */

#include "HardwareSerial.h"

__xdata uint8_t
    Receive_Uart0_Buf[SERIAL0_RX_BUFFER_SIZE]; // arduino style serial buffer
__xdata uint8_t
    Transmit_Uart0_Buf[SERIAL0_TX_BUFFER_SIZE]; // arduino style serial buffer
volatile __xdata uint8_t uart0_rx_buffer_head = 0;
volatile __xdata uint8_t uart0_rx_buffer_tail = 0;
volatile __xdata uint8_t uart0_tx_buffer_head = 0;
volatile __xdata uint8_t uart0_tx_buffer_tail = 0;
volatile __bit uart0_flag_sending = 0;

void uart0IntRxHandler() {
  __data uint8_t nextHead = (uart0_rx_buffer_head + 1) % SERIAL0_RX_BUFFER_SIZE;

  if (nextHead != uart0_rx_buffer_tail) {
    Receive_Uart0_Buf[uart0_rx_buffer_head] = SBUF;
    uart0_rx_buffer_head = nextHead;
  }
}

void uart0IntTxHandler() {
  if (uart0_flag_sending) {
    if (uart0_tx_buffer_head == uart0_tx_buffer_tail) {
      // do no more
      uart0_flag_sending &= 0;
    } else {
      SBUF = Transmit_Uart0_Buf[uart0_tx_buffer_tail];
      uart0_tx_buffer_tail =
          (uart0_tx_buffer_tail + 1) % SERIAL0_TX_BUFFER_SIZE;
    }
  }
}
