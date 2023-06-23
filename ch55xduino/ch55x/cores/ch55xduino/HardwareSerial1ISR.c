#include "HardwareSerial.h"

__xdata uint8_t
    Receive_Uart1_Buf[SERIAL1_RX_BUFFER_SIZE]; // arduino style serial buffer
__xdata uint8_t
    Transmit_Uart1_Buf[SERIAL1_TX_BUFFER_SIZE]; // arduino style serial buffer
volatile __xdata uint8_t uart1_rx_buffer_head = 0;
volatile __xdata uint8_t uart1_rx_buffer_tail = 0;
volatile __xdata uint8_t uart1_tx_buffer_head = 0;
volatile __xdata uint8_t uart1_tx_buffer_tail = 0;
volatile __bit uart1_flag_sending = 0;

void uart1IntRxHandler() {
  __data uint8_t nextHead = (uart1_rx_buffer_head + 1) % SERIAL1_RX_BUFFER_SIZE;

  if (nextHead != uart1_rx_buffer_tail) {
#if defined(CH551) || defined(CH552)
    Receive_Uart1_Buf[uart1_rx_buffer_head] = SBUF1;
#elif defined(CH559)
    Receive_Uart1_Buf[uart1_rx_buffer_head] = SER1_RBR;
#elif defined(CH549)
    Receive_Uart1_Buf[uart1_rx_buffer_head] = SBUF1;
#endif
    uart1_rx_buffer_head = nextHead;
  }
}

void uart1IntTxHandler() {
  if (uart1_flag_sending) {
    if (uart1_tx_buffer_head == uart1_tx_buffer_tail) {
      // do no more
      uart1_flag_sending &= 0;
    } else {
#if defined(CH551) || defined(CH552)
      SBUF1 = Transmit_Uart1_Buf[uart1_tx_buffer_tail];
#elif defined(CH559)
      SER1_THR = Transmit_Uart1_Buf[uart1_tx_buffer_tail];
#elif defined(CH549)
      SBUF1 = Transmit_Uart1_Buf[uart1_tx_buffer_tail];
#endif
      uart1_tx_buffer_tail =
          (uart1_tx_buffer_tail + 1) % SERIAL1_TX_BUFFER_SIZE;
    }
  }
}
