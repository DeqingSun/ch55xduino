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

#pragma callee_saves sendCharDebug
void sendCharDebug(char c);

void Timer2Interrupt(void) __interrupt {
  if (TF2) {
    TF2 = 0;
    __data signed char highByteOfPCM = Ep1Buffer[(soundBufferPlayBackIndex<<1)+1];
    sendCharDebug(highByteOfPCM);
    __data unsigned char highByteOfPCMUnsigned = highByteOfPCM+128;
    PWM_DATA2 = highByteOfPCMUnsigned;
    soundBufferPlayBackIndex++;
  }
}

void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared

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