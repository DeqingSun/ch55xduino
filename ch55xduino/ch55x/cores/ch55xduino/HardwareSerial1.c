#include "HardwareSerial.h"

__xdata unsigned char serial1Initialized;

extern __xdata uint8_t Receive_Uart1_Buf[];  // arduino style serial buffer
extern __xdata uint8_t Transmit_Uart1_Buf[]; // arduino style serial buffer
extern volatile __xdata uint8_t uart1_rx_buffer_head;
extern volatile __xdata uint8_t uart1_rx_buffer_tail;
extern volatile __xdata uint8_t uart1_tx_buffer_head;
extern volatile __xdata uint8_t uart1_tx_buffer_tail;
extern volatile __bit uart1_flag_sending;

// extern wait functions
void delayMicroseconds(__data uint16_t us);

uint8_t Serial1(void) { return serial1Initialized; }

void Serial1_begin(__data unsigned long baud) {

#if defined(CH551) || defined(CH552)
  U1SM0 = 0;
  U1SMOD = 1; // use mode 1 for serial 1
  U1REN = 1;  // Enable serial 1 receive

  SBAUD1 = 256 - F_CPU / 16 / baud;

  IE_UART1 = 1;
  EA = 1; // Enable serial 1 interrupt
#elif defined(CH559)
  __data uint32_t x;
  __data uint8_t x2;
  SER1_LCR |= bLCR_DLAB; // change baudrate
  SER1_DIV = 1;
  x = 10 * F_CPU * 2 / 1 / 16 / baud;
  x2 = ((uint16_t)x) % 10;
  x /= 10;
  if (x2 >= 5)
    x++; // round
  SER1_DLM = x >> 8;
  SER1_DLL = x & 0xff;
  SER1_LCR &= ~bLCR_DLAB;                   // prevent changing baudrate
  XBUS_AUX |= bALE_CLK_EN;                  // make RS485EN = 0
  SER1_LCR = bLCR_WORD_SZ1 | bLCR_WORD_SZ0; // no break, no parity, 8n1
  SER1_IER =
      bIER_PIN_MOD1 | bIER_THR_EMPTY | bIER_RECV_RDY; // RXD1:P2.6 TXD1:P2.7
  SER1_MCR |= bMCR_OUT2;
  IE_UART1 = 1;
  EA = 1;
#elif defined(CH549)
  SCON1 = bU1REN | bU1SMOD;
  SBAUD1 = 256 - F_CPU / 16 / baud;
  SIF1 = bU1TI | bU1RI; // clear interrupt flags
  IE_UART1 = 1;
  EA = 1;
#endif
  serial1Initialized = 1;
}

uint8_t Serial1_write(__data uint8_t SendDat) {
  __data uint8_t interruptOn = EA;
  EA = 0;

  if ((uart1_tx_buffer_head == uart1_tx_buffer_tail) &&
      (uart1_flag_sending == 0)) { // start to send
    uart1_flag_sending = 1;
#if defined(CH551) || defined(CH552)
    SBUF1 = SendDat;
#elif defined(CH559)
    SER1_THR = SendDat;
#elif defined(CH549)
    SBUF1 = SendDat;
#endif
    if (interruptOn)
      EA = 1;
    return 1;
  }

  __data uint8_t nextHeadPos =
      ((uint8_t)(uart1_tx_buffer_head + 1)) % SERIAL1_TX_BUFFER_SIZE;

  __data uint16_t waitWriteCount = 0;
  while ((nextHeadPos == uart1_tx_buffer_tail)) { // wait max 100ms or discard
    if (interruptOn)
      EA = 1;
    waitWriteCount++;
    delayMicroseconds(5);
    if (waitWriteCount >= 20000)
      return 0;
  }
  Transmit_Uart1_Buf[uart1_tx_buffer_head] = SendDat;

  uart1_tx_buffer_head = nextHeadPos;

  if (interruptOn)
    EA = 1;

  return 1;
}

void Serial1_flush(void) {
  while (uart1_flag_sending)
    ;
}

uint8_t Serial1_available(void) {
  __data uint8_t rxBufLength =
      ((uint8_t)(SERIAL1_RX_BUFFER_SIZE + uart1_rx_buffer_head -
                 uart1_rx_buffer_tail)) %
      SERIAL1_RX_BUFFER_SIZE;
  return rxBufLength;
}

uint8_t Serial1_read(void) {
  __data uint8_t rxBufLength =
      ((uint8_t)(SERIAL1_RX_BUFFER_SIZE + uart1_rx_buffer_head -
                 uart1_rx_buffer_tail)) %
      SERIAL1_RX_BUFFER_SIZE;
  if (rxBufLength > 0) {
    __data uint8_t result = Receive_Uart1_Buf[uart1_rx_buffer_tail];
    uart1_rx_buffer_tail =
        (((uint8_t)(uart1_rx_buffer_tail + 1)) % SERIAL1_RX_BUFFER_SIZE);
    return result;
  }
  return 0;
}
