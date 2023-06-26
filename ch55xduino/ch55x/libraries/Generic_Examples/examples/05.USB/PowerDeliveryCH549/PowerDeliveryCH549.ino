#if F_CPU != 32000000
#error "This only run for 32M clock"
#endif

#include "src/pd.h"

__xdata uint8_t status;
__xdata uint16_t ERR;
__xdata uint8_t Connect_Status;





void PD_Init( )
{
  P1_MOD_OC &= ~(bUCC2 | bUCC1);
  P1_DIR_PU &= ~(bUCC2 | bUCC1);                                                   //UCC1 UCC2 Set to float input

  P2_MOD_OC &= ~(5 << 2);                                                          //LED1 LED2 Push pull output
  P2_DIR_PU |= (5 << 2);

  USB_C_CTRL |= bUCC1_PD_EN | bUCC2_PD_EN;                                         //CC1 pulldown 5.1K (ext?)
  CCSel = 1;                                                                       //choose CC1
  USB_C_CTRL |= bUCC_PD_MOD;                                                       //BMC Output enable

  ADC_CFG |= (bADC_EN | bADC_AIN_EN | bVDD_REF_EN | bCMP_EN);                      //enable ADC power, open ext channel, open comparator and reference power, NEG of comp using 1/8VDD
  ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //ADC lock set to 750K
  ADC_CTRL = bADC_IF;                                                              //Clear ADC conversion finish flag
  delayMicroseconds(2);                                                                     //wait till ADC power to be stablea
  delayMicroseconds(2);
}

uint8_t Connect_Check( void )
{
  __data uint16_t UCCn_Value;
  for (__data uint8_t i = 1; i <= 2; i++) {
    ADC_CTRL = bADC_IF;
    ADC_CHAN = 4 - 1 + i;                                                               //CC1 connect to AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //start sampling
    while ((ADC_CTRL & bADC_IF) == 0);                                              //check finish flag
    ADC_CTRL = bADC_IF;                                                             //clear flag
    UCCn_Value = ADC_DAT & 0xFFF;
    //  printf("UCC1=%d\n",(UINT16)UCC1_Value);

    if (UCCn_Value > DefaultPowerMin)
    {
      CCSel = i;
      return DFP_PD_CONNECT;
    }
  }
  return DFP_PD_DISCONNECT;
}





void setup() {
  //delay(1000);
  PD_Init();
  Serial0_begin(115200);
  delay(20);
  Serial0_println("Type-C DP start ...");

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  P1_7 = 0; //!!!!!!
  //SndMsgID = 0;//!!!!???????
}

void loop() {
  /* Check the connection of DFP */
  status = Connect_Check();
  if ( status != DFP_PD_DISCONNECT )                                     //DFP connected
  {
    ERR = 0;
    if (Connect_Status == 0)
    {
      Serial0_print("Con");
      Serial0_print(CCSel);
      Connect_Status = 1;
    }
  }
  else                                                                   //DFP disconnected
  {
    if (Connect_Status)                                                //Clear status
    {
      ERR++;
      if (ERR == 1000)
      {
        ERR = 0;
        Serial0_println("DCon\n");
        Connect_Status = 0;
      }
    }
  }

  if (Connect_Status)                                                    //when connected, process data receiving
  {
    P1_6 = 1;//!!!!!!

    {
      //set T0
      TR0 = 0;
      ET0 = 0;
      T2MOD |= (bTMR_CLK) | (bT0_CLK);
      TMOD = (TMOD & ~0x0F) | (bT0_M1); // mode 2 for autoreload
      TH0 = 0x96;
      TL0 = 0x96;

    }
    /*status = */ReceiveHandle();

    { //restore T0
      TR0 = 0;
      TMOD = (TMOD & ~0x0F) | (bT0_M1); // mode 2 for autoreload
      T2MOD = T2MOD & ~bT0_CLK;         // bT0_CLK=0;clk Div by 12
      TH0 = 255 - 250 + 1;
      TF0 = 0;
      ET0 = 1;
      TR0 = 1;
    }

    P1_6 = 0;//!!!!!!

    if (RcvDataCount > 0) {
      for (uint8_t i = 0; i < RcvDataCount; i++) {
        USBSerial_print(RcvDataBuf[i], HEX);
        USBSerial_print(" ");
      }
      USBSerial_println();
      RcvDataCount = 0;
    }
  }

}
