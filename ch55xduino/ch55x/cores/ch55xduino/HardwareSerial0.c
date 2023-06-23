/*
 created by Deqing Sun for use with CH55xduino
 */

#include "HardwareSerial.h"

__xdata unsigned char serial0Initialized;

extern __xdata uint8_t Receive_Uart0_Buf[];  // arduino style serial buffer
extern __xdata uint8_t Transmit_Uart0_Buf[]; // arduino style serial buffer
extern volatile __xdata uint8_t uart0_rx_buffer_head;
extern volatile __xdata uint8_t uart0_rx_buffer_tail;
extern volatile __xdata uint8_t uart0_tx_buffer_head;
extern volatile __xdata uint8_t uart0_tx_buffer_tail;
extern volatile __bit uart0_flag_sending;

// extern wait functions
void delayMicroseconds(__data uint16_t us);

uint8_t Serial0(void) { return serial0Initialized; }

void Serial0_begin(__data unsigned long baud) {

  __data uint32_t x;
  __data uint8_t x2;

  x = 10 * F_CPU / baud / 16; // Make sure it doesn't overflow when baudrate is
                              // changed, default 9600
  x2 = ((uint16_t)x) % 10;
  x /= 10;
  if (x2 >= 5)
    x++; // round

  SM0 = 0;
  SM1 = 1;
  SM2 = 0; // use mode 0 for serial 0
  // use Timer1 for baudrate generator
  RCLK = 0; // UART0 receive clk
  TCLK = 0; // UART0 send clk
  PCON |= SMOD;

  TMOD = TMOD & ~bT1_GATE & ~bT1_CT & ~MASK_T1_MOD |
         bT1_M1; // 0X20,Timer1 as 8 bit autoload timer
  T2MOD = T2MOD | bTMR_CLK |
          bT1_CLK; // Timer1 clk selection. trade off: bTMR_CLK is low, will
                   // make Uart0 not accurate. bTMR_CLK is high, make T2 timeout
                   // is short, multiple timeout must be allowed in 1st edge, as
                   // reset may take long
  TH1 = 0 - x;     // baud/12 is real rate
  TR1 = 1;         // start timer1
  TI = 0;
  REN = 1; // Enable serial 0 receive

  ES = 1; // Enable serial 0 interrupt

  serial0Initialized = 1;
}

uint8_t Serial0_write(__data uint8_t SendDat) {
  __data uint8_t interruptOn = EA;
  EA = 0;
  if ((uart0_tx_buffer_head == uart0_tx_buffer_tail) &&
      (uart0_flag_sending == 0)) { // start to send
    uart0_flag_sending = 1;
    SBUF = SendDat;
    if (interruptOn)
      EA = 1;
    return 1;
  }

  __data uint8_t nextHeadPos =
      ((uint8_t)(uart0_tx_buffer_head + 1)) % SERIAL0_TX_BUFFER_SIZE;

  __data uint16_t waitWriteCount = 0;
  while ((nextHeadPos == uart0_tx_buffer_tail)) { // wait max 100ms or discard
    if (interruptOn)
      EA = 1;
    waitWriteCount++;
    delayMicroseconds(5);
    if (waitWriteCount >= 20000)
      return 0;
  }
  Transmit_Uart0_Buf[uart0_tx_buffer_head] = SendDat;

  uart0_tx_buffer_head = nextHeadPos;

  if (interruptOn)
    EA = 1;

  return 1;
}

void Serial0_flush(void) {
  while (uart0_flag_sending)
    ;
}

uint8_t Serial0_available(void) {
  __data uint8_t rxBufLength =
      ((uint8_t)(SERIAL0_RX_BUFFER_SIZE + uart0_rx_buffer_head -
                 uart0_rx_buffer_tail)) %
      SERIAL0_RX_BUFFER_SIZE;
  return rxBufLength;
}

uint8_t Serial0_read(void) {
  __data uint8_t rxBufLength =
      ((uint8_t)(SERIAL0_RX_BUFFER_SIZE + uart0_rx_buffer_head -
                 uart0_rx_buffer_tail)) %
      SERIAL0_RX_BUFFER_SIZE;
  if (rxBufLength > 0) {
    __data uint8_t result = Receive_Uart0_Buf[uart0_rx_buffer_tail];
    uart0_rx_buffer_tail =
        (((uint8_t)(uart0_rx_buffer_tail + 1)) % SERIAL0_RX_BUFFER_SIZE);
    return result;
  }
  return 0;
}
