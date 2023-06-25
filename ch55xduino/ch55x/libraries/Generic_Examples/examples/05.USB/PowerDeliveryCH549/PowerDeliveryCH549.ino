#if F_CPU != 32000000
#error "This only run for 32M clock"
#endif

/* Á¬½Ó×´Ì¬ */
#define  DFP_PD_CONNECT    0x00
#define  DFP_PD_DISCONNECT 0x01

/* CC½Å¼ì²âµ½µÄADCÖµÅÐ¶Ï¹©µçÄÜÁ¦ */
#define  DefaultPowerMin  (342-137)
#define  DefaultPowerMax  (342+158)
#define  Power1_5AMin     (771-198)
#define  Power1_5AMax     (771+180)
#define  Power3_0AMin     (1383-310)
#define  Power3_0AMax     (1383+288)

__data uint8_t CCSel;
__xdata uint8_t status;
__xdata uint16_t ERR;
__xdata uint8_t Connect_Status;
__xdata uint8_t RcvDataBuf[73];
__xdata uint8_t RcvDataCount;



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

void CMP_Interrupt() {
  //clear RcvDataBuf[73]
  __asm__(
    "    inc _XBUS_AUX                            \n"
    "    mov r7, #73                              \n"
    "    clr a                                    \n"
    "    mov dptr,#(_RcvDataBuf)                  \n"
    "loop_clr_RcvDataBuf$:                        \n"
    "    .db #0xa5                                \n"
    "    djnz r7,loop_clr_RcvDataBuf$             \n"
    "    dec _XBUS_AUX                            \n"
  );

  __data uint8_t preambleFlag = 1;
  __data uint8_t bitmask = 3;
  __data uint8_t tempA = 0;

  TL0 = 0;
  TH0 = 0;
  TF0 = 0;
  TR0 = 1;
  RcvDataCount = 0;
  //for BMC@300K,each 0 is 3.33us and 1 is 1.66us*2
  do {
    TL0 = 0;
    ADC_CTRL = bCMP_IF;
    while ((ADC_CTRL & bCMP_IF) == 0) {
      //121@32M = 3.78us, longer than a regular bit
      if (TL0 >= 121) {
        return;
      }
    }
    //87@32M = 2.72us, try until we find a bit 0
  } while (TL0 < 87);
  //now we are at an end of bit 0
  TL0 = 0;
  ADC_CTRL = bCMP_IF;
  do {
    //another transition!
    if ((ADC_CTRL & bCMP_IF)) {
      TL0 = 0;
      ADC_CTRL = bCMP_IF;
      do {
        //wait for another transition
        while ((ADC_CTRL & bCMP_IF) == 0) {
          //I guess this is faster than compare?
          if (TF0) {
            TR0 = 0;
            TF0 = 0;
            return;
          }
        }
        //since we are look for preamble, we are sure we are at end of bit 1.
        //if there are 2 of 0 then the code will not work, but it is not preamble anyway.
        TL0 = 0;
        ADC_CTRL = bCMP_IF;
        RcvDataBuf[0] = (RcvDataBuf[0] << 1) + tempA;
        //last 2 bit are 00 or 10, that is the end of preamble
        //All SOP start with Sync-1 K-code 11000, at LSB first, that is 00
        //Cable Reset and Hard Reset start with RST-1 K-code 00111, can ignore.
        //not sure why we need to check the case of 10
        if ((RcvDataBuf[0] == 0x54) || (RcvDataBuf[0] == 0x56)) {
          RcvDataBuf[0] = RcvDataBuf[0] & 3;
          preambleFlag = 0;
        }
        //wait 1.56us, if it is 1 we should already passed the center
        while (TL0 < 0x32);
        tempA = ((ADC_CTRL & bCMP_IF) != 0);
        if (tempA) {
          ADC_CTRL = bCMP_IF;
        }
      } while (preambleFlag != 0);
      //now we are 2 bits beyond preamble
      do {
        while ((ADC_CTRL & bCMP_IF)) {
          TL0 = 0;
          ADC_CTRL = 0x40;
          RcvDataBuf[RcvDataCount] = (RcvDataBuf[RcvDataCount] << 1) + tempA;
          //each byte contains 5bits.
          bitmask--;
          if (bitmask == 0) {
            bitmask = 5;
            RcvDataCount++;
          }
          //wait 1.56us, if it is 1 we should already passed the center
          while (TL0 < 0x32);
          if ((ADC_CTRL & bCMP_IF) == 0) {
            tempA = false;
          }
          else {
            ADC_CTRL = bCMP_IF;
            tempA = true;
          }
        }
      } while (TF0 == 0);
      TF0 = 0;
      TR0 = 0;
      return;
    }
    //as long as we are shorter than a regular bit
  } while (TL0 < 121);
}

uint8_t ReceiveHandle() {
  __xdata uint16_t TimeOutCount;
  //DAT_INTMEM_65 = CC_Sel;
  ADC_CHAN = CCSel + 3 | 0x30;
  TimeOutCount = 0;
  ADC_CTRL = bCMP_IF;
  while ((ADC_CTRL & bCMP_IF) == 0) {
    TimeOutCount++;
    if (TimeOutCount >= 700) {  //need more investigation what time it means. ( (34+3+(2or3))*700 clks? 0.85ms? 256bits of BMC?) this SDCC code need more tuning
      return 1;
    }
  }
  E_DIS = 1;
  CMP_Interrupt();
  E_DIS = 0;


  //delay(1);
  return 1;
}

void setup() {
  //delay(1000);
  PD_Init();
  Serial0_begin(115200);
  delay(20);
  Serial0_println("Type-C DP start ...");

  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  P1_7 = 0;
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
    P1_6 = 1;

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

    P1_6 = 0;

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
