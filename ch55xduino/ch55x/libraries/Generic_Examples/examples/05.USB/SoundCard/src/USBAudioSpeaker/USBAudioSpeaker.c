// clang-format off
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USBAudioSpeaker.h"
// clang-format on

#define STR_INDIR(x) #x
#define STR(x) STR_INDIR(x)

#pragma callee_saves sendCharDebug
void sendCharDebug(char c);

__xdata volatile uint8_t soundBufferPlayBackIndex = 0;

//variables for zero crossing detection
__xdata volatile uint8_t lastSampleAboveZero = 0;
__xdata volatile uint8_t countInbetweenZeroCrossing = 0;
__xdata volatile uint8_t zeroCrossingCount = 0;
__xdata volatile uint8_t zeroCrossingBufferIndex = 0;
__xdata volatile uint8_t zeroCrossingBuffer[ZERO_CROSSING_BUFFER_SIZE];

#define T2_RELOAD_VALUE (65536 - F_CPU / 24000)
#define T2_RELOAD_VALUE_LOW (T2_RELOAD_VALUE & 0xFF)
#define T2_RELOAD_VALUE_HIGH ((T2_RELOAD_VALUE >> 8) & 0xFF)
#define T2_RELOAD_VALUE_NEAR_FINISH (65536 - F_CPU / 24000 / 4)
#define T2_RELOAD_VALUE_NEAR_FINISH_LOW (T2_RELOAD_VALUE_NEAR_FINISH & 0xFF)
#define T2_RELOAD_VALUE_NEAR_FINISH_HIGH                                       \
  ((T2_RELOAD_VALUE_NEAR_FINISH >> 8) & 0xFF)

void Timer2Interrupt(void) __interrupt {
  //   if (TF2) {
  //     TF2 = 0;
  //     __data signed char highByteOfPCM =
  //     Ep1Buffer[(soundBufferPlayBackIndex<<1)+1];
  //     __data unsigned char highByteOfPCMUnsigned = highByteOfPCM+128;

  //     PWM_CTRL |= bPWM_IF_END;
  //     while (!(PWM_CTRL & bPWM_IF_END));
  //     PWM_DATA2 = highByteOfPCMUnsigned;
  //     soundBufferPlayBackIndex++;
  //   }

  // due to PWM bug, https://github.com/DeqingSun/ch55xduino/issues/135
  // we need to carefully handle the PWM data
  // and SDCC is not good at handling partial inline assembly.
  // we just do everything in inline assembly.
  __asm__(
          "    push ar7                            \n"
          "    push ar6                            \n"
    
    
          ";check if TF2 is set                    \n"
          "    jb   _TF2,notSkipTimer2INTR$        \n"
          "    ajmp skipTimer2INTR$                \n"
          "notSkipTimer2INTR$:                     \n"
          "    clr  _TF2                           \n"

          ";get (soundBufferPlayBackIndex<<1)+1    \n"
          ";also inc soundBufferPlayBackIndex      \n"
          "    mov dptr,#_soundBufferPlayBackIndex \n"
          "    movx a,@dptr                        \n"
          "    mov r7,a                            \n"
          ";check if soundBufferPlayBackIndex is too big \n"
          "    add a,#232                          \n"
          "    jnc soundBufferPlayBackIndexNotBig$ \n"
          "    ajmp skipTimer2INTR$                \n"
          "soundBufferPlayBackIndexNotBig$:        \n"
          "    mov a,r7                            \n"
          "    inc a                               \n"
          "    movx @dptr,a                        \n"
          "    mov a,r7                            \n"
          "    rl	a                                \n"
          "    inc a                               \n"

          ";get Ep1Buffer[(soundBufferPlayBackIndex<<1)+1] \n"
          "    add a,#_Ep1Buffer                   \n"
          "    mov dpl,a                           \n"
          "    clr a                               \n"
          "    addc a,#(_Ep1Buffer >> 8)           \n"
          "    mov dph,a                           \n"
          "    movx a,@dptr                        \n"

          ";add 128 for PWM                        \n"
          "    mov r7,a                            \n"
          "    mov a,#0x80                         \n"
          "    add a,r7                            \n"
          "    mov r7,a                            \n"

          ";load previous PWM                      \n"
          "    mov a,_PWM_DATA2                    \n"
          "    add a,#232                          \n"
          "    jnc previousPWMSmall$               \n"
          ";if previous PWM was 20~25              \n"
          ";we have chance to update PWM on the falling edge \n"
          ";and that triggers bug of PWM           \n"
          ";so we just wait after falling in that case \n"

          ";previousPWM not Small, just set at beginning \n"
          ";clear bPWM_IF_END                      \n"
          "    orl _PWM_CTRL,#0x10                 \n"
          ";wait for bPWM_IF_END                   \n"
          "waitPWM_IF_END$:                        \n"
          "    mov a,_PWM_CTRL                     \n"
          "    jnb acc.4,waitPWM_IF_END$           \n"
          "    mov a,r7                            \n"
          "    mov  _PWM_DATA2,a                   \n"
          "    sjmp pwmSetFinish$                  \n"

          "previousPWMSmall$:                      \n"
          ";clear bPWM_IF_END                      \n"
          "    orl _PWM_CTRL,#0x10                 \n"
          ";wait for bPWM_IF_END                   \n"
          "waitPWM_IF_END_small$:                  \n"
          "    mov a,_PWM_CTRL                     \n"
          "    jnb acc.4,waitPWM_IF_END_small$     \n"
          "    mov a,r7                            \n"
          "    nop\n nop\n nop\n nop\n nop\n         "
          "    nop\n nop\n nop\n nop\n nop\n         "
          "    nop\n nop\n nop\n nop\n nop\n         "
          "    nop\n nop\n nop\n nop\n nop\n         "
          "    nop\n nop\n nop\n nop\n nop\n         "

          "    mov  _PWM_DATA2,a                   \n"

          "pwmSetFinish$:                          \n"

  // at this point, R7 is the data to be played back
  // maybe we do not need Goertzel, we just need to do zero-crossing detection

          "    mov dptr,#_lastSampleAboveZero      \n"
          "    movx	a,@dptr                        \n"
          "    jnz  lookForFallingCrossing$        \n"

          "lookForRisingCrossing$:                 \n"
          "    mov a,r7                            \n"
          ";check if the value is bigger or equal than 128+2\n"
          "    add a,#(256-128-2)                  \n"
          "    jnc   notCrossing$                  \n"
          ";found rising, will track falling next\n"
          "    mov dptr,#_lastSampleAboveZero      \n"
          "    mov a,#1                            \n"
          "    movx @dptr,a                        \n"
          "    sjmp crossing$                      \n"

          "lookForFallingCrossing$:                \n"
          "    mov a,r7                            \n"
          ";check if the value is less or equal than 128-2\n"
          "    add a,#(256-128+1)                  \n"
          "    jc   notCrossing$                   \n"
          ";found falling, will track rising next  \n"
          "    mov dptr,#_lastSampleAboveZero      \n"
          "    mov a,#0                            \n"
          "    movx @dptr,a                        \n"
          "    sjmp crossing$                      \n"

          "notCrossing$:                           \n"
          "    mov dptr,#_countInbetweenZeroCrossing\n"
          "    movx a,@dptr                        \n"
          "    add a,#1                            \n"
          "    movx @dptr,a                        \n"
          "    jnc notCrossingNoTooLong$           \n"
          //todo: we need to reset the zero crossing detection here


          "notCrossingNoTooLong$:                  \n"


          "    sjmp endOfCrossingDetection$        \n"

          "crossing$:                              \n"
          "; increase zeroCrossingCount            \n"
          "    mov dptr,#_zeroCrossingCount        \n"
          "    movx a,@dptr                        \n"
          "    inc a                               \n"
          "    movx @dptr,a                        \n"

          "    mov dptr,#_countInbetweenZeroCrossing\n"
          "    movx a,@dptr                        \n"
          "    mov r6,a                            \n"
          "    clr a                               \n"
          "    movx @dptr,a                        \n"

          "    mov dptr,#_zeroCrossingBufferIndex  \n"
          "    movx a,@dptr                        \n"
          ";reuse r7 to store the index            \n"
          "    mov r7,a                            \n"
          "    inc a                               \n"
          "    anl a,#("STR(ZERO_CROSSING_BUFFER_SIZE)"-1)\n"
          "    movx @dptr,a                        \n"  

          "    mov dptr,#_zeroCrossingBuffer       \n"
          "    mov a,r7                            \n"     
          "    add a,dpl                           \n"
          "    mov dpl,a                           \n"
          "    mov a,dph                           \n"
          "    addc a,#0                           \n"
          "    mov dph,a                           \n"
          "    mov a,r6                            \n"
          "    movx @dptr,a                        \n"


          "endOfCrossingDetection$:                \n"



          "skipTimer2INTR$:                        \n"

          "    pop ar6                             \n"
          "    pop ar7                             \n"
          
          
          
          );


}

void USBInit() {
  USBDeviceCfg();         // Device mode configuration
  USBDeviceEndPointCfg(); // Endpoint configuration
  USBDeviceIntCfg();      // Interrupt configuration
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0; // Pre-use send length must be cleared

  for (uint8_t i = 0; i < 48; i++) {
    Ep1Buffer[i] = 0;
  }

  T2CON = 0x00;
  // bTMR_CLK may be set by uart0, we keep it as is.
  T2MOD |= bTMR_CLK | bT2_CLK; // use Fsys for T2

  TL2 = 0;
  TH2 = 0;
  RCAP2L = T2_RELOAD_VALUE_LOW;
  RCAP2H = T2_RELOAD_VALUE_HIGH;

  ET2 = 1;
  TR2 = 1;
}

void AUDIO_EP1_Out(void) {

  // Force T2 interrupt to sync with the data received
  TR2 = 0;
  TF2 = 0;
  // the last data will be played back during the new data is being received
  // just make sure the last data is played back before the new last data is
  // received
  TL2 = T2_RELOAD_VALUE_NEAR_FINISH_LOW;
  TH2 = T2_RELOAD_VALUE_NEAR_FINISH_HIGH;
  soundBufferPlayBackIndex = 0;
  TR2 = 1;

  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_TOUT;
}