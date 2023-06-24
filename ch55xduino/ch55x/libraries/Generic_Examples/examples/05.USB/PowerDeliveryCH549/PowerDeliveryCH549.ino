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

__xdata uint8_t CCSel;
__xdata uint8_t status;
__xdata uint16_t ERR;
__xdata uint8_t Connect_Status;

void PD_Init( )
{
  P1_MOD_OC &= ~(bUCC2 | bUCC1);
  P1_DIR_PU &= ~(bUCC2 | bUCC1);                                                   //UCC1 UCC2 Set to float input

  P2_MOD_OC &= ~(5 << 2);                                                          //LED1 LED2 Push pull output
  P2_DIR_PU |= (5 << 2);

  USB_C_CTRL |= bUCC1_PD_EN;                                                       //CC1 pulldown 5.1K (ext?)
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
  uint16_t UCCn_Value;

  ADC_CHAN = 4;                                                                   //CC1 connect to AIN4(P14)
  ADC_CTRL = bADC_START;                                                          //start sampling
  while ((ADC_CTRL & bADC_IF) == 0);                                              //check finish flag
  ADC_CTRL = bADC_IF;                                                             //clear flag
  UCCn_Value = ADC_DAT & 0xFFF;
  //  printf("UCC1=%d\n",(UINT16)UCC1_Value);

  if (UCCn_Value > DefaultPowerMin)
  {
    return DFP_PD_CONNECT;
  }
  else
  {
    return DFP_PD_DISCONNECT;
  }
}

uint8_t ReceiveHandle(uint8_t ccsel){
  ADC_CHAN = ccsel + 3 | 0x30;
  delay(1);
  return 1;
}

void setup() {
  //delay(1000);
  PD_Init();
  Serial0_begin(115200);
  delay(20);
  Serial0_println("Type-C DP start ...");

}

void loop() {
  /* ¼ì²âDFPµÄÁ¬½Ó£¬CC1Í¨Ñ¶ */
  status = Connect_Check();
  if ( status != DFP_PD_DISCONNECT )                                     //DFPÁ¬½Ó
  {
    ERR = 0;
    if (Connect_Status == 0)
    {
      Serial0_println("Con\n");
      Connect_Status = 1;
    }
  }
  else                                                                   //DFPÎ´Á¬½Ó
  {
    if (Connect_Status)                                                //Çå³ý×´Ì¬
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

  if (Connect_Status)                                                    //ÒÑÁ¬½Ó£¬´Ë´¦Êý¾Ý½ÓÊÕ´¦Àí
  {
    status = ReceiveHandle( CCSel );
  }

}
