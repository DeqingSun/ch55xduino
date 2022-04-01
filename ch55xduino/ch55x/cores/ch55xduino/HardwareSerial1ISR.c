#include "HardwareSerial.h"

__xdata uint8_t Receive_Uart1_Buf[SERIAL1_RX_BUFFER_SIZE];   //arduino style serial buffer
__xdata uint8_t Transmit_Uart1_Buf[SERIAL1_TX_BUFFER_SIZE];   //arduino style serial buffer
volatile __xdata uint8_t uart1_rx_buffer_head=0;
volatile __xdata uint8_t uart1_rx_buffer_tail=0;
volatile __xdata uint8_t uart1_tx_buffer_head=0;
volatile __xdata uint8_t uart1_tx_buffer_tail=0;
volatile __bit uart1_flag_sending=0;

void uart1IntRxHandler(){
    uint8_t nextHead = (uart1_rx_buffer_head + 1) % SERIAL1_RX_BUFFER_SIZE;

    if (nextHead != uart1_rx_buffer_tail) {
        Receive_Uart1_Buf[uart1_rx_buffer_head] = SBUF1;
        uart1_rx_buffer_head = nextHead;
    }
}

void uart1IntTxHandler(){
    if (uart1_flag_sending){
        if (uart1_tx_buffer_head == uart1_tx_buffer_tail){
            //do no more
            uart1_flag_sending &= 0;
        }else{
            SBUF1=Transmit_Uart1_Buf[uart1_tx_buffer_tail];
            uart1_tx_buffer_tail = (uart1_tx_buffer_tail + 1) % SERIAL1_TX_BUFFER_SIZE;
        }
    }
}


