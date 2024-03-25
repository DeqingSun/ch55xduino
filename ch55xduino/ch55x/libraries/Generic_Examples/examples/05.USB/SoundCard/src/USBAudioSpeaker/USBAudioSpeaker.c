#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
#include "USBhandler.h"
#include "USBAudioSpeaker.h"

__xdata uint32_t curAddr, endAddr;

void USBInit(){
    USBDeviceCfg();                                                       //Device mode configuration
    USBDeviceEndPointCfg();                                               //Endpoint configuration   
    USBDeviceIntCfg();                                                    //Interrupt configuration    
    UEP0_T_LEN = 0;
    UEP1_T_LEN = 0;                                                       //Pre-use send length must be cleared
}

void Mass_Storage_Out (void) {
    PWM_CTRL |= bPWM2_OUT_EN;
	for (uint8_t i = 0; i < BOT_EP_Rx_Length; i=i+4){		
		PWM_DATA2 = BOT_Rx_Buf[i+1];
		// Delay for 20833ns
		for (uint16_t j=0;j<51;j++) {
		   __asm__ ("nop\n");
		}
		//T2COUNT = 0;
		//mTimer2RunCTL(START);
		//while (T2COUNT<400) {
		//}
    }
	PWM_CTRL &= (~bPWM2_OUT_EN);
    //Serial0_println("Ending");
	BOT_EP_Tx_ISO_Valid();
}