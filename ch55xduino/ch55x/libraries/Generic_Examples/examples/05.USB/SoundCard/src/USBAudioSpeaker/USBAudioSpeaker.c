#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USBAudioSpeaker.h"

__xdata volatile uint8_t soundBufferPlayBackIndex = 0;

#define T2_RELOAD_VALUE (65536-F_CPU/24000)
#define T2_RELOAD_VALUE_LOW (T2_RELOAD_VALUE & 0xFF)
#define T2_RELOAD_VALUE_HIGH ((T2_RELOAD_VALUE >> 8) & 0xFF)
#define T2_RELOAD_VALUE_NEAR_FINISH (65536-F_CPU/24000/4)
#define T2_RELOAD_VALUE_NEAR_FINISH_LOW (T2_RELOAD_VALUE_NEAR_FINISH & 0xFF)
#define T2_RELOAD_VALUE_NEAR_FINISH_HIGH ((T2_RELOAD_VALUE_NEAR_FINISH >> 8) & 0xFF)

void Timer2Interrupt(void) __interrupt {
    //   if (TF2) {
    //     TF2 = 0;
    //     __data signed char highByteOfPCM = Ep1Buffer[(soundBufferPlayBackIndex<<1)+1];
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
    __asm__(";check if TF2 is set                    \n"
            "    jb   _TF2,notSkipTimer2INTR$        \n"
            "    sjmp skipTimer2INTR$                \n"
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
            "    sjmp skipTimer2INTR$                \n"
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

            "skipTimer2INTR$:                        \n"
            );

}

void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared

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

void AUDIO_EP1_Out (void) {

    //Force T2 interrupt to sync with the data received
    TR2 = 0;
    TF2 = 0;
    //the last data will be played back during the new data is being received
    //just make sure the last data is played back before the new last data is received
    TL2 = T2_RELOAD_VALUE_NEAR_FINISH_LOW;
    TH2 = T2_RELOAD_VALUE_NEAR_FINISH_HIGH;
    soundBufferPlayBackIndex = 0;
    TR2 = 1;

	UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_TOUT;
}