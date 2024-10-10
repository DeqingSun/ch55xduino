// clang-format off
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"

#include "debugWireFunc.h"
#include "hardwareShare.h"
#include "USBhandler.h"
// clang-format on

/*extern volatile __xdata uint8_t dwBuf[128];
extern volatile uint8_t dwLen;        // Length being received from host or avr
device extern volatile uint8_t dwState;      // Current debugWIRE action
underway, 0 if none extern volatile __xdata uint16_t dwBitTime;      // Each bit
takes 4*dwBitTime+8 cycles to transmit extern volatile __xdata uint16_t
dwselfCalcBitTime; extern volatile uint8_t dwTXbitCount; extern volatile uint8_t
dwTXRXPtr; extern volatile uint8_t dwInterrputStatus; extern volatile uint16_t
dwRead1stBitT2Val;*/

// ----------------------------------------------------------------------
volatile __data uint8_t jobState = 0;
// ----------------------------------------------------------------------
// debugWIRE support
volatile __xdata uint8_t dwBuf[128];
volatile __data uint8_t dwLen; // Length being received from host or avr device
volatile __data uint8_t dwState =
    0; // Current debugWIRE action underway, 0 if none
volatile __xdata uint16_t
    dwBitTime; // Each bit takes 4*dwBitTime+8 cycles to transmit
volatile __data uint16_t dwselfCalcBitTime; // Each bit use T2 to transmit
volatile __data uint16_t dwselfCalcBitTimeForRCAP2; // Each bit use T2 to transmit
volatile __data uint8_t dwUsbIOFinishedLen = 0;
volatile __data uint8_t dwIOStatus = 0;
volatile __data uint8_t dwInterrputStatus = 0;
volatile __data uint8_t dwTXbitCount;
volatile __data uint8_t dwTXRXBuf;
volatile __data uint8_t dwTXRXPtr;
volatile __data uint16_t dwRead1stBitT2Val;

__data uint8_t dwBreakDelayCounter = 0;

__xdata uint8_t debugWireAutoQuit =
    0; // if there is no signal back on MISO in ISP, we do a DW quit operation
       // automatically

/* ------------------------------------------------------------------------- */
/* ------------------------------- DebugWIRE ------------------------------- */
/* ------------------------------------------------------------------------- */

// RS232 levels are
//
//   5v ttl  =  mark   =  1  =  idle   =  stop-bit   =  -3v..-15v line
//   0v ttl  =  space  =  0  =  break  =  start-bit  =  +3v..+15v line
//
//   Least significant bit sent first.

// extern uint16_t dwBitTime;      // Each bit takes 4*dwBitTime+8 cycles to
// transmit

void dwCaptureWidths() { // TODO: Seems only capture max 4ms wait. Not working
                         // for 65ms bootd delay yet.

  // called functions are free to use registers

  // setup T2 for capture
  TR2 = 0;
  RCLK = 0;
  TCLK = 0;  // clear RCLK,TCLK in T2CON
  C_T2 = 0;  // clear C_T2 in T2CON for using internal clk
  EXEN2 = 1; // set EXEN2 in T2CON for T2EX trigger
  // T2MOD&=~(bT2_CLK);                //clear bT2_CLK,Using Fsys/12
  T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/1
  T2MOD &=
      ~(bT2_CAP_M0 |
        bT2_CAP_M1); // Set bT2_CAP_M1 to 0, bT2_CAP_M0 to 1, catch any edge
  T2MOD |= ((bT2_CAP_M0));
  CP_RL2 = 1; // set CP_RL2 in T2CON for capture in RCAP2
  TL2 = 0;
  TH2 = 0;
  EXF2 = 0;

  EA = 0; // disable Interrupt

  __asm__(
      "; This is a comment\n"
      "    nop                    \n"
      "    mov dph,#(_dwBuf>>8)                    \n"
      "    mov dpl,#(_dwBuf)                        \n"
      "    mov r0,#0                                \n" // r0 r1 is cache for T2
      "    mov r1,#0                                \n"
      "    mov r2,#0                                \n" // r2 r3 is cache for
                                                        // RCAP2
      "    mov r3,#0                                \n"
      "    mov r4,#0                                \n" // r4 r5 is
                                                        // prevTriggerVal
      "    mov r5,#0                                \n"
      // r6 is temp
      "    mov r7,#0                                \n" // Counter for number of
                                                        // measured pulses
      "    anl _P1_DIR_PU,#0xfd                    \n" // P1_DIR_PU &= ~( (1<<1)
                                                       // );//End break ,set
                                                       // reset to input
      "    setb    _TR2                            \n"

      "waitInitSignalUp$:                            \n" // wait till signal
                                                         // jump up
      "    mov a,_TH2                                \n" // quit if wait for too
                                                         // long
      "   clr c                                    \n"
      "   subb a,#0xFE                            \n"
      "   jnc captureRoutineFinished$                \n"
      "    mov    a,_P1                                \n" // p1.1 must be high
                                                           // to proceed
      "    jnb    acc.1,waitInitSignalUp$                \n"
      "    clr    _EXF2                                \n"

      "; The first value doesn't matter, just wait for more than 1 overflow \n"
      "; each overflow is 2.7ms, let's do 15 \n"

      "    clr    _TF2                                \n"
      "    mov a,#15                                \n"
      "capture1stWaitLoop$:                        \n"
      "    jb _EXF2,edgeChangeTrigger$                \n"
      "    jnb _TF2,capture1stWaitLoop$             \n"
      "    clr _TF2                                \n"
      "    dec a                                     \n"
      "    jnz capture1stWaitLoop$                     \n"
      "    ljmp captureRoutineFinished$            \n"

      "captureWaitLoop$:                            \n"
      "    jnb _EXF2,skipTriggerRoutine$            \n"
      "edgeChangeTrigger$:                        \n"
      "    mov r2,_RCAP2L                            \n" // load _RCAP2L
      "    mov r3,_RCAP2H                            \n"
      "    clr    _EXF2                                \n"
      "    mov a,r2                                   \n" // Record low pulse
                                                          // time (RCAP2
                                                          // cache-prevTriggerVal)
      "    clr c                                    \n"
      "    subb a,r4                                \n"
      "    movx @DPTR,a                            \n"
      "    mov a,r3                                 \n"
      "    subb a,r5                                \n"
      "    inc DPTR                                \n"
      "    movx @DPTR,a                            \n"
      "    inc DPTR                                \n"
      "    inc r7                                    \n"

      "    mov a,r7                                   \n"
      "    add a,#(256-64)                            \n"
      "    jz captureRoutineFinished$                \n"

      "    mov a,r2                                \n" // prevTriggerVal = RCAP2
                                                       // cache
      "    mov r4,a                                \n"
      "    mov a,r3                                \n"
      "    mov r5,a                                \n"
      "skipTriggerRoutine$:                        \n"

      "loadT2Loop$:                                \n"
      "    mov r1,_TH2                                \n" // load TH2, then TL2,
                                                          // if TH2 changes, do
                                                          // it again
      "    mov r0,_TL2                                \n"
      "    mov a,r1                                \n"
      "    cjne a,_TH2,loadT2Loop$                    \n"

      "    mov a,r0                                \n" // calc T2cache -
                                                       // prevTriggerVal
      "    clr c                                    \n"
      "    subb a,r4                                \n"
      "    mov b,a                                    \n" // keep low 8bit
      "    mov a,r1                                \n"
      "    subb a,r5                                \n"
      "    mov r6,a                                \n"
      "    mov a,b                                    \n"
      "    clr c                                    \n"
      "    subb a,#(65000&0xFF)                                \n" // only carry
                                                                   // result is
                                                                   // needed
      "    mov a,r6                                \n"
      "    subb a,#(65000>>8)                                \n"
      "   jc captureWaitLoop$                        \n" // if (T2cache -
                                                         // prevTriggerVal)<65000,
                                                         // go back

      "captureRoutineFinished$:                    \n"
      "    mov a,r7                                  \n" // Convert word count
                                                         // to byte count
      "    rl a                                      \n"
      "    mov _dwLen,a                               \n"

  );

  EA = 1; // enable Interrupt
  TR2 = 0;

  // calc dwselfCalcBitTime
  // dwBuf was used as temp buffer, storing the pulse width for 9 falling/rising
  // after break signal. we do an average of the pulse width, and then convert
  // it to bit time.
  {
    __xdata uint32_t dwSum; // same iRam
    __data uint8_t i;
    dwSum = 0;
    if (dwLen >= 18) {
      for (i = (dwLen - 18); i < (dwLen); i += 2) {
        /// pointer physically in data ram pointing to xdata
        dwSum = dwSum + *((__xdata uint16_t * __data)(&dwBuf[i]));
      }
      dwselfCalcBitTime = dwSum / 9;
      dwselfCalcBitTimeForRCAP2 = 65535 - dwselfCalcBitTime + 1;
      // for 24M CH552 and 16M Arduino Uno. The datarate is 125K and the
      // dwselfCalcBitTime is mesured to be 0xC1 So 24M/(193-1) = 125K.
    }

    // send calcuted value to mimic attiny
    // the SetDwireBaud in dwire would do 16.5M/(6*data+8) for baud rate

    for (i = 0; i < (dwLen); i += 2) {
      __xdata uint16_t *__data dataPtr = ((uint16_t *)(&dwBuf[i]));
#define CLOCK_IN_MHZ (F_CPU / 1000000)
      *dataPtr = ((33 * ((*dataPtr)) / (2 * CLOCK_IN_MHZ)) - 8) / 6;
    }
  }
}

void dwSendBytesBlocking() {
  // dwselfCalcBitTime;
  __data uint8_t i;
  __data uint8_t sendByte;

  TR2 = 0;

  RCLK = 0;
  TCLK = 0;           // clear RCLK,TCLK in T2CON
  C_T2 = 0;           // clear C_T2 in T2CON for using internal clk
  EXEN2 = 0;          // clear EXEN2 in T2CON to disable T2EX
  T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/4
  CP_RL2 = 0;         // clear CP_RL2 in T2CON for 16bit timer, reload mode
  RCAP2 = dwselfCalcBitTimeForRCAP2;
  // init output
  P1_1 = 1;
  P1_DIR_PU |= (1 << 1);

  EA = 0; // disable Interrupt

  TF2 = 0;
  EXF2 = 0;

  TR2 = 1; // start timer

  TL2 = 0x00; // seem only work when timer is on?
  TH2 = 0xFF;

  while (!TF2)
    ;
  TF2 = 0; // wait for 1 bit to avoid issue of start bit.

  for (i = 0; i < dwLen; i++) {
    sendByte = dwBuf[i];
    while (!TF2)
      ;
    TF2 = 0;
    P1_1 = 0;
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 0)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 1)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 2)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 3)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 4)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 5)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 6)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    if (sendByte & (1 << 7)) {
      P1_1 = 1;
    } else {
      P1_1 = 0;
    };
    while (!TF2)
      ;
    TF2 = 0;
    P1_1 = 1;
  }

  P1_DIR_PU &= ~(1 << 1);
  P1_1 = 1;

  EA = 1; // enable Interrupt
  TR2 = 0;
}

void dwReadBytesBlocking() {

  __data uint8_t receiveByte;
  dwLen = 0;

  TR2 = 0;

  RCLK = 0;
  TCLK = 0;           // clear RCLK,TCLK in T2CON
  C_T2 = 0;           // clear C_T2 in T2CON for using internal clk
  EXEN2 = 0;          // clear EXEN2 in T2CON to disable T2EX
  T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/4
  CP_RL2 = 0;         // clear CP_RL2 in T2CON for 16bit timer, reload mode
  RCAP2 = 65535 - (dwselfCalcBitTime >> 1) + 1; // double speed to capture

  P1_DIR_PU &= ~(1 << 1);

  EA = 0; // disable Interrupt

  TF2 = 0;
  EXF2 = 0;

  TR2 = 1; // start timer

  TL2 = 0x00; // seem only work when timer is on?
  TH2 = 0x00;
  TF2 = 0;

  while (1) {
    receiveByte = 0;
    while (1) {
      if ((P1_1) == 0) {
        break; // first fall edge
      }
      if (TF2)
        goto endOfDwReceive; // time out
    }
    TF2 = 0;
    TR2 = 0;
    __asm__("; compensate for lost time\n" // t2 = rcap2+n
            "    mov a,_RCAP2L                    \n"
            "    add a,#10                       \n"
            "    mov _TL2,a                    \n"
            "    mov a,_RCAP2H                    \n"
            "    addc a,#0                   \n"
            "    mov _TH2,a                    \n");
    TF2 = 0;
    TR2 = 1;

    while (TF2 == 0)
      ;
    TF2 = 0; // center of start bit

    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 0 bit
    if ((P1_1)) {
      receiveByte |= (1 << 0);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 1 bit
    if ((P1_1)) {
      receiveByte |= (1 << 1);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 2 bit
    if ((P1_1)) {
      receiveByte |= (1 << 2);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 3 bit
    if ((P1_1)) {
      receiveByte |= (1 << 3);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 4 bit
    if ((P1_1)) {
      receiveByte |= (1 << 4);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 5 bit
    if ((P1_1)) {
      receiveByte |= (1 << 5);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 6 bit
    if ((P1_1)) {
      receiveByte |= (1 << 6);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    while (TF2 == 0)
      ;
    TF2 = 0; // center of 7 bit
    if ((P1_1)) {
      receiveByte |= (1 << 7);
    };
    while (TF2 == 0)
      ;
    TF2 = 0;
    dwBuf[dwLen] = receiveByte;
    dwLen++;
    while (TF2 == 0)
      ;
    TF2 = 0; // we shall be in center of stopbit

    // set timer to wait for 1 bit for next byte
    TF2 = 0;
    TR2 = 0;
    __asm__("; set timer to wait for 1 bit\n" // 65535-dwselfCalcBitTime
            "    mov a,#0xFF                        \n"
            "    clr c                              \n"
            "    subb a,_dwselfCalcBitTime                       \n"
            "    mov _TL2,a                    \n"
            "    mov a,#0xFF                    \n"
            "    subb a,_dwselfCalcBitTime+1                       \n"
            "    mov _TH2,a                    \n");
    TF2 = 0;
    TR2 = 1;
  }

endOfDwReceive:

  EA = 1; // enable Interrupt
}

void dwWaitForBitInterruptInit() {
  dwBuf[0] = 0;
  dwLen = 1;
  P1_DIR_PU &= ~((1 << 1));

  // SET Timer 2 to 16bit counter, just as catcher
  TR2 = 0;
  EXEN2 = 1; // connect EXF2
  TF2 = 0;
  EXF2 = 0;
  ET2 = 1;
  //    P3_3=1; //turn on LED
}

void dwSendBytesInterruptInit() {
  TR2 = 0;

  RCLK = 0;
  TCLK = 0;           // clear RCLK,TCLK in T2CON
  C_T2 = 0;           // clear C_T2 in T2CON for using internal clk
  EXEN2 = 0;          // clear EXEN2 in T2CON to disable T2EX
  T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/4
  CP_RL2 = 0;         // clear CP_RL2 in T2CON for 16bit timer, reload mode
  RCAP2 = dwselfCalcBitTimeForRCAP2;
  dwRead1stBitT2Val = 65535 - dwselfCalcBitTime - (dwselfCalcBitTime >> 1) +
                      171; // make some compensation
  // init output
  P1_1 = 1;
  P1_DIR_PU |= (1 << 1);

  TF2 = 0;
  EXF2 = 0;

  dwTXbitCount = 0;
  dwTXRXPtr = 0;

  TR2 = 1; // start timer

  TL2 = 0x00; // seem only work when timer is on?
  TH2 = 0xFF;

  ET2 = 1;
}

void dwReadBytesInterruptInit() {
  dwLen = 0;

  TR2 = 0;

  RCLK = 0;
  TCLK = 0; // clear RCLK,TCLK in T2CON
  C_T2 = 0; // clear C_T2 in T2CON for using internal clk
  T2MOD &=
      ~(bT2_CAP_M0 |
        bT2_CAP_M1); // Set bT2_CAP_M1 to 0, bT2_CAP_M0 to 0, catch falling edge
  EXEN2 = 1;         // set EXEN2 in T2CON to enable T2EX
  T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/4
  CP_RL2 = 0;         // clear CP_RL2 in T2CON for 16bit timer, reload mode
  RCAP2 = dwselfCalcBitTimeForRCAP2; // single speed to capture

  P1_DIR_PU &= ~(1 << 1);

  dwTXbitCount = 0xFF;
  dwTXRXPtr = 0;

  TF2 = 0;
  EXF2 = 0;

  TR2 = 1; // start timer

  TL2 = 0x00; // seem only work when timer is on?
  TH2 = 0x00;
  TF2 = 0;

  ET2 = 1;
}

void dwSendBytesInterrupt() {
  dwInterrputStatus = DWIO_SEND_BYTES;
  dwSendBytesInterruptInit();
}

void dwSendReadBytesInterrupt() {
  dwInterrputStatus = DWIO_SEND_BYTES | DWIO_READ_BYTES;
  dwSendBytesInterruptInit();
}

void dwReadBytesInterrupt() {}

void dwSendBytesWaitInterrupt() {
  dwInterrputStatus = DWIO_SEND_BYTES | DWIO_WAIT_FOR_BIT;
  dwSendBytesInterruptInit();
}

void dwWaitForBitInterrupt() {
  dwInterrputStatus = DWIO_WAIT_FOR_BIT;
  dwWaitForBitInterruptInit();
}

void Timer2Interrupt(void) __interrupt(INT_NO_TMR2) {
  if (TF2) {
    TF2 = 0;
    if (dwInterrputStatus & DWIO_SEND_BYTES) { // send bytes
      if (dwTXbitCount == 0) {
        P1_1 = 1;
        if (dwLen == dwTXRXPtr) { // end if transmittion
          P1_DIR_PU &= ~(1 << 1);
          ET2 = 0;
          dwInterrputStatus &= ~DWIO_SEND_BYTES;
          if (dwInterrputStatus == 0) { // write only
            dwInterrputStatus = (uint8_t)DWIO_FINISHED;
          } else if (dwInterrputStatus & DWIO_WAIT_FOR_BIT) {
            //dwWaitForBitInterruptInit();
            //copy code here for optimization
            dwBuf[0] = 0;
            dwLen = 1;
            P1_DIR_PU &= ~((1 << 1));

            // SET Timer 2 to 16bit counter, just as catcher
            TR2 = 0;
            EXEN2 = 1; // connect EXF2
            TF2 = 0;
            EXF2 = 0;
            ET2 = 1;
          } else if (dwInterrputStatus & DWIO_READ_BYTES) {
            //dwReadBytesInterruptInit();
            //copy code here for optimization
            //transit from send to receive takes 9.58us in 16M clock, longer than 8us but timer stops
            dwLen = 0;

            TR2 = 0;

            RCLK = 0;
            TCLK = 0; // clear RCLK,TCLK in T2CON
            C_T2 = 0; // clear C_T2 in T2CON for using internal clk
            T2MOD &=
                ~(bT2_CAP_M0 |
                  bT2_CAP_M1); // Set bT2_CAP_M1 to 0, bT2_CAP_M0 to 0, catch falling edge
            EXEN2 = 1;         // set EXEN2 in T2CON to enable T2EX
            T2MOD |= (bT2_CLK); // set bT2_CLK, for fast clk.Using Fsys/4
            CP_RL2 = 0;         // clear CP_RL2 in T2CON for 16bit timer, reload mode
            RCAP2 = dwselfCalcBitTimeForRCAP2; // single speed to capture

            P1_DIR_PU &= ~(1 << 1);

            dwTXbitCount = 0xFF;
            dwTXRXPtr = 0;

            TF2 = 0;
            EXF2 = 0;

            TR2 = 1; // start timer

            TL2 = 0x00; // seem only work when timer is on?
            TH2 = 0x00;
            TF2 = 0;

            ET2 = 1;
          }
        } else {
          dwTXRXBuf = dwBuf[dwTXRXPtr];
          dwTXRXPtr++;
          dwTXbitCount = 9;
        }
      } else if (dwTXbitCount <= 8) {
        //send a bit takes 5.7us in 16M clock 
        if (dwTXRXBuf & (1 << 0)) {
          P1_1 = 1;
        } else {
          P1_1 = 0;
        }
        dwTXRXBuf >>= 1;
        dwTXbitCount--;
      } else {
        P1_1 = 0; // start bit
        dwTXbitCount--;
      }

    } else if (dwInterrputStatus & DWIO_READ_BYTES) {
      if (dwTXbitCount == 0xFF) { // not get the first bit, timeout
        EXEN2 = 0;
        ET2 = 0;
        dwInterrputStatus &= ~DWIO_READ_BYTES;
        if (dwInterrputStatus == 0) { // write only
          dwInterrputStatus = (uint8_t)DWIO_FINISHED;
        }
      } else if (dwTXbitCount >= 8) {

        EXEN2 = 1;    // ready to capture next falling edge
        TL2 = RCAP2L; // give timer a bit extra time to capture
        TH2 = RCAP2H;
        dwTXbitCount = 0xFF;

      } else {
        // read bit takes 6.26us in 16M clock
        // __data uint8_t dataBufferRead = P1_1;
        // dwTXRXBuf >>= 1;
        // if (dataBufferRead)
        //   dwTXRXBuf |= (1 << 7);
        __asm__("; shift P1_1 to dwTXRXBuf\n" // 65535-dwselfCalcBitTime
                "    mov	c,_P1_1                       \n"
                "    mov	a,_dwTXRXBuf                  \n"
                "    rrc	a                             \n"
                "    mov	_dwTXRXBuf,a                  \n"
                );
        if (dwTXbitCount == 7) {
          dwBuf[dwLen] = dwTXRXBuf;
          dwLen++;
        }
        dwTXbitCount++;
      }
    }

  } else if (EXF2) {
    EXF2 = 0;
    if (dwInterrputStatus & DWIO_WAIT_FOR_BIT) {
      dwBuf[0] = 1;
      dwInterrputStatus &= ~DWIO_WAIT_FOR_BIT;
      if (dwInterrputStatus == 0) { // write only
        dwInterrputStatus = (uint8_t)DWIO_FINISHED;
      }
      EXEN2 = 0;
      ET2 = 0;
    }
    if (dwInterrputStatus &
        DWIO_READ_BYTES) { // this is 1st pin change in Read bytes
      //transit from send to receive takes 5.57us in 16M clock
      EXEN2 = 0;
      dwTXbitCount = 0;
      TF2 = 0;
      TR2 = 0;

      TL2 = dwRead1stBitT2Val;
      TH2 = dwRead1stBitT2Val >> 8;

      dwTXRXBuf = 0;

      TF2 = 0;
      TR2 = 1;

    } else {
      // there must be some error
      ET2 = 0;
    }
  }
}

uint16_t debugWireSetupHandler() {
  __data uint16_t returnLen;
  dwUsbIOFinishedLen = 0;
  if (dwState) {
    returnLen = 0; // Prior operation has not yet completed
  } else {
    if (Ep0Buffer[0] & 0x80) {
      // IN transfer - device to host: return buffered data
      returnLen = dwLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : dwLen;
      // memcpy(Ep0Buffer,dwBuf+dwUsbIOFinishedLen,returnLen);
      for (__data uint8_t i = 0; i < returnLen; i++) {
        ((uint8_t *)Ep0Buffer)[i] = ((uint8_t *)dwBuf)[i + dwUsbIOFinishedLen];
      }
      dwUsbIOFinishedLen += returnLen;
      usbMsgFlags |= USB_FLG_DW_IN;
    } else {
      // OUT transfer - host to device. rq->wValue specifies action to take.
      dwState = Ep0Buffer[2]; // action required, from low byte of rq->wValue
      dwLen = *((uint16_t *)(Ep0Buffer + 6)); // rq->wLength
      returnLen = 0;
      if (dwLen == 0) {
        jobState = 20; // No out data transfer, go straight to job part
      } else {
        if (dwLen > sizeof(dwBuf))
          dwLen = sizeof(dwBuf);
        returnLen = dwLen >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : dwLen;
        usbMsgFlags |= USB_FLG_DW_OUT;
      }
    }
  }
  return returnLen;
}

void debugWireInHandler() {
  __data uint8_t returnLen;
  __data uint8_t dwLeftOver;
  if ((usbMsgFlags & USB_FLG_DW_IN)) {
    dwLeftOver = dwLen - dwUsbIOFinishedLen;
    returnLen =
        dwLeftOver >= DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : dwLeftOver;
    // memcpy(Ep0Buffer,dwBuf+dwUsbIOFinishedLen,len);
    for (__data uint8_t i = 0; i < returnLen; i++) {
      ((uint8_t *)Ep0Buffer)[i] = ((uint8_t *)dwBuf)[i + dwUsbIOFinishedLen];
    }
    dwUsbIOFinishedLen += returnLen;
    UEP0_T_LEN = returnLen;
    UEP0_CTRL ^= bUEP_T_TOG;
  } else {
    UEP0_T_LEN = 0; // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
  }
}

void debugWireOutHandler() {
  __data uint8_t returnLen;
  __data uint8_t dwLeftOver;
  if ((usbMsgFlags & USB_FLG_DW_OUT)) {
    dwLeftOver = dwLen - dwUsbIOFinishedLen;
    if (dwLeftOver <= DEFAULT_ENDP0_SIZE) { // isLastBlock
      returnLen = dwLeftOver;
      jobState = 20;
    } else {
      returnLen = DEFAULT_ENDP0_SIZE;
    }
    // memcpy(dwBuf+dwUsbIOFinishedLen,Ep0Buffer,len);
    for (__data uint8_t i = 0; i < returnLen; i++) {
      ((uint8_t *)dwBuf)[i + dwUsbIOFinishedLen] = ((uint8_t *)Ep0Buffer)[i];
    }
    dwUsbIOFinishedLen += returnLen;
    UEP0_T_LEN = (dwUsbIOFinishedLen < dwLen) ? returnLen : 0;
    UEP0_CTRL ^= bUEP_R_TOG;
  } else {
    UEP0_T_LEN = 0;
    UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_ACK; // 状态阶段，对IN响应NAK
  }
}

void debugWireAutoQuitProcess() {
  switch (debugWireAutoQuit) {
  case DWAQ_NORMAL:
    return;
    break;
  case DWAQ_NOSIGNAL_ISP_QUIT:
    if (dwState) {
      return; // Prior operation has not yet completed
    } else {
      dwState = 0x21;
      dwLen = 18; // auto calculate rate
      jobState = 20;
      debugWireAutoQuit = DWAQ_CAPTURING_WIDTH;
      // start to send a break and capture rate
    }
    break;
  case DWAQ_CAPTURING_WIDTH:
    if (jobState == 0) {
      dwState = 0x04;
      dwLen = 1;
      dwBuf[0] = 0x06; // Disable dW, this enables ISP.
      jobState = 20;
      debugWireAutoQuit = DWAQ_SENDING_QUIT;
      // start to send Disable dW command
    }
  case DWAQ_SENDING_QUIT:
    if (jobState == 0) {
      debugWireAutoQuit = DWAQ_NORMAL;
    }
  default:
    break;
  }
}

void debugWireProcess() {

  debugWireAutoQuitProcess();

  if ((dwIOStatus != DWIO_NONE) && ((dwIOStatus & DWIO_FINISHED) == 0)) {
    // dwState flag bits:
    //
    //     00000001   0x01     Send break
    //     00000010   0x02     Set timing parameter
    //     00000100   0x04     Send bytes
    //     00001000   0x08     Wait for start bit
    //     00010000   0x10     Read bytes
    //     00100000   0x20     Read pulse widths
    //
    // Supported combinations
    //    $21 - Send break and read pulse widths
    //    $02 - Set timing parameters
    //    $04 - Send bytes
    //    $0C - Send bytes and wait for dWIRE line state change
    //    $14 - Send bytes and read response (normal command)
    //    $24 - Send bytes and receive 0x55 pulse widths
    //
    // Note that the wait for start bit loop also monitors the dwState wait for
    // start bit flag, and is arranged so that sending a 33 (send break and read
    // pulse widths) will abort a pending wait. (not implemented yet)
    if (dwIOStatus & DWIO_SEND_BREAK) {
      if (dwBreakDelayCounter == 0) {
        P1_1 = 0;
        P1_DIR_PU |= ((1 << 1));
      } else {
        delayMicroseconds(1000);
        if (dwBreakDelayCounter >= 100) {
          dwIOStatus &= ~DWIO_SEND_BREAK;
        }
      }
      dwBreakDelayCounter++;
    } else if (dwIOStatus & DWIO_SET_TIMING) {
      ((char *)&dwBitTime)[0] = dwBuf[0];
      ((char *)&dwBitTime)[1] = dwBuf[1];
      dwIOStatus &= ~DWIO_SET_TIMING;
    } else if (dwIOStatus & DWIO_SEND_BYTES) {

      // is there any combination?
      if (dwIOStatus & DWIO_WAIT_FOR_BIT) {
        if (dwInterrputStatus == 0) {
          dwSendBytesWaitInterrupt();
        } else if ((dwInterrputStatus & DWIO_SEND_BYTES) == 0) {
          // let interrupt running
          // dwInterrputStatus = 0;
          dwIOStatus &= ~(DWIO_SEND_BYTES | DWIO_WAIT_FOR_BIT);
        }
      } else if (dwIOStatus & DWIO_READ_BYTES) {
        if (dwInterrputStatus == 0) {
          dwSendReadBytesInterrupt();
        } else if (dwInterrputStatus & DWIO_FINISHED) {
          dwInterrputStatus = 0;
          dwIOStatus &= ~(DWIO_SEND_BYTES | DWIO_READ_BYTES);
        }
      } else if (dwIOStatus & DWIO_READ_PULSE_WIDTH) {
        if (dwInterrputStatus == 0) {
          dwSendBytesInterrupt();
        } else if ((dwInterrputStatus & DWIO_SEND_BYTES) == 0) {
          dwCaptureWidths(); // to break down
          dwInterrputStatus = 0;
          dwIOStatus &= ~(DWIO_SEND_BYTES | DWIO_READ_PULSE_WIDTH);
        }
      } else { // send byte only
        if (dwInterrputStatus == 0) {
          dwSendBytesInterrupt();
        } else if (dwInterrputStatus & DWIO_FINISHED) {
          dwInterrputStatus = 0;
          dwIOStatus &= ~DWIO_SEND_BYTES;
        }
      }

    } else if (dwIOStatus & DWIO_WAIT_FOR_BIT) {
      if (dwInterrputStatus == 0) {
        dwWaitForBitInterrupt();
      } else if (dwInterrputStatus & DWIO_FINISHED) {
        dwInterrputStatus = 0;
        dwIOStatus &= ~DWIO_WAIT_FOR_BIT;
      }
    } else if (dwIOStatus & DWIO_READ_BYTES) {
      if (dwInterrputStatus == 0) {
        dwSendReadBytesInterrupt();
      } else if (dwInterrputStatus & DWIO_FINISHED) {
        dwInterrputStatus = 0;
        dwIOStatus &= ~(DWIO_READ_BYTES);
      }
    } else if (dwIOStatus & DWIO_READ_PULSE_WIDTH) {
      dwCaptureWidths(); // to break down
      dwIOStatus &= ~DWIO_READ_PULSE_WIDTH;
    }

    if ((dwIOStatus & 0x3F) == 0) {
      dwIOStatus = (uint8_t)DWIO_FINISHED;
    }

  } else { // wait for new command from USB request
    switch (jobState) {
    case 0: /* idle ... */
      break;
    case 20: // DebugWIRE. dwState determines action:

      if (dwIOStatus & DWIO_FINISHED) {
        jobState = 0;
        dwState = 0;
        dwIOStatus = 0;
        ledState &= ~LED_FLG_DEBUGWIRE;
        SET_LED_WITH_MASK();
      } else {
        dwBreakDelayCounter = 0;
        dwInterrputStatus = 0;
        dwIOStatus =
            dwState &
            ~(DWIO_FINISHED); // make sure the DWIO_FINISHED is cleared.
        ledState |= LED_FLG_DEBUGWIRE;
        SET_LED_WITH_MASK();
      }

      break;

    default:
      jobState = 0;
      break;
    }
  }
}
