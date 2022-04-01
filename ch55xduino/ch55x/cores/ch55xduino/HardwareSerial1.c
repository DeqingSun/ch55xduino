#include "HardwareSerial.h"

__xdata unsigned char serial1Initialized;

extern __xdata uint8_t Receive_Uart1_Buf[];   //arduino style serial buffer
extern __xdata uint8_t Transmit_Uart1_Buf[];   //arduino style serial buffer
extern volatile __xdata uint8_t uart1_rx_buffer_head;
extern volatile __xdata uint8_t uart1_rx_buffer_tail;
extern volatile __xdata uint8_t uart1_tx_buffer_head;
extern volatile __xdata uint8_t uart1_tx_buffer_tail;
extern volatile __bit uart1_flag_sending;

//extern wait functions
void delayMicroseconds(uint16_t us);

uint8_t Serial1(void){
    return serial1Initialized;
}

void Serial1_begin(unsigned long baud){
    U1SM0 = 0;
    U1SMOD = 1;                                                                  //use mode 1 for serial 1
    U1REN = 1;                                                                   //Enable serial 1 receive

    SBAUD1 = 256 - F_CPU / 16 / baud;

    IE_UART1 = 1;
    EA = 1;                                                                       //Enable serial 1 interrupt

    serial1Initialized = 1;
}

uint8_t Serial1_write(uint8_t SendDat)
{
    if ( (uart1_tx_buffer_head == uart1_tx_buffer_tail) && (uart1_flag_sending==0) ){    //start to send
        uart1_flag_sending = 1;
        SBUF1 = SendDat;
        return 1;
    }

    uint8_t nextHeadPos =  ((uint8_t)(uart1_tx_buffer_head + 1)) % SERIAL1_TX_BUFFER_SIZE;

    uint16_t waitWriteCount=0;
    while ((nextHeadPos == uart1_tx_buffer_tail) ){    //wait max 100ms or discard
        waitWriteCount++;
        delayMicroseconds(5);
        if (waitWriteCount>=20000) return 0;
    }
    Transmit_Uart1_Buf[uart1_tx_buffer_head]=SendDat;

    uart1_tx_buffer_head = nextHeadPos;

    return 1;
}

void Serial1_flush(void){
    while( uart1_flag_sending );
}

uint8_t Serial1_available(void){
    uint8_t rxBufLength = ((uint8_t)(SERIAL1_RX_BUFFER_SIZE + uart1_rx_buffer_head - uart1_rx_buffer_tail)) % SERIAL1_RX_BUFFER_SIZE;
    return rxBufLength;
}

uint8_t Serial1_read(void){
    uint8_t rxBufLength = ((uint8_t)(SERIAL1_RX_BUFFER_SIZE + uart1_rx_buffer_head - uart1_rx_buffer_tail)) % SERIAL1_RX_BUFFER_SIZE;
    if(rxBufLength>0){
        uint8_t result = Receive_Uart1_Buf[uart1_rx_buffer_tail];
        uart1_rx_buffer_tail = (((uint8_t)(uart1_rx_buffer_tail + 1)) % SERIAL1_RX_BUFFER_SIZE);
        return result;
    }
    return 0;
}
