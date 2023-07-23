#include "src/pd.h"

__xdata uint8_t status;
__xdata uint16_t ERR;
__xdata uint8_t Connect_Status;

//testing voltage not too high
#define TARGET_VOLT_MV (7000)
#define ALLOWED_MIN_VOLT_MV (5000)
#define ALLOWED_MAX_VOLT_MV (20000)

void setT0ForCC() {
  TR0 = 0;
  ET0 = 0;
  T2MOD |= (bTMR_CLK) | (bT0_CLK);
  TMOD = (TMOD & ~0x0F) | (bT0_M1); // mode 2 for autoreload
}

void restoreT0ForTiming() {
  TR0 = 0;
  TMOD = (TMOD & ~0x0F) | (bT0_M1); // mode 2 for autoreload
  T2MOD = T2MOD & ~bT0_CLK;         // bT0_CLK=0;clk Div by 12
  TH0 = 255 - 250 + 1;
  TF0 = 0;
  ET0 = 1;
  TR0 = 1;
}


void PD_Init( )
{
#if defined(CH549)
  P1_MOD_OC &= ~(bUCC2 | bUCC1);
  P1_DIR_PU &= ~(bUCC2 | bUCC1);                                                   //UCC1 UCC2 Set to float input

  USB_C_CTRL |= bUCC1_PD_EN | bUCC2_PD_EN;                                         //CC1 pulldown 5.1K (ext?)
  CCSel = 1;                                                                       //choose CC1
  USB_C_CTRL |= bUCC_PD_MOD;                                                       //BMC Output enable, on CH549, this make output under 1.2V

  ADC_CFG |= (bADC_EN | bADC_AIN_EN | bVDD_REF_EN | bCMP_EN);                      //enable ADC power, open ext channel, open comparator and reference power, NEG of comp using 1/8VDD
  ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //ADC lock set to 750K
  ADC_CTRL = bADC_IF;                                                              //Clear ADC conversion finish flag
  delayMicroseconds(2);                                                            //wait till ADC power to be stable
#elif defined(CH552)

  USB_C_CTRL |= bUCC1_PD_EN | bUCC2_PD_EN;

  P1_MOD_OC &= ~((1 << 4) | (1 << 5));
  P1_DIR_PU &= ~((1 << 4) | (1 << 5));

  //voltage divider on P3.2 to get 0.6V
  P3_MOD_OC &= ~(1 << 2);
  P3_DIR_PU &= ~(1 << 2);

  //P1.4 connect to CC1 directly with internal 5.1K pull-down. If 1K resistor is inserted, the signal will be too weak
  //P3.4 connect 3 (0.7+0.3+0.3 underdrive) diode to CC. Switch between Input and GND to hold voltage on 1V
  //P1.7 connect to CC via 1K resistor (limit current) and diode to P1.7 (increase pulldown strength)

  P3_MOD_OC &= ~(1 << 4); //!!!!! using P3.4 for control for now
  P3_DIR_PU &= ~(1 << 4); //!!!!! using P3.4 for control for now
  P3 &= ~(1 << 4); //!!!!! using P3.4 for control for now
  //P1.7 for drive
  P1_DIR_PU &= ~(1 << 7);
  P1_DIR_PU &= ~(1 << 7);

  CCSel = 1;                                                                       //choose CC1
  ADC_CFG = bADC_EN | bCMP_EN | bADC_CLK;
  ADC_CTRL = 0;
  CMP_CHAN = 1;
  delayMicroseconds(2);                                                            //wait till ADC power to be stable
#else
#error "MCU type not tested"
#endif
}

uint8_t Connect_Check( void )
{
#if defined(CH549)
  __data uint16_t UCCn_Value;
#elif defined(CH552)
  __data uint8_t UCCn_Value;
#endif
  for (__data uint8_t i = 1; i <= 2; i++) {
#if defined(CH549)
    ADC_CTRL = bADC_IF;
    ADC_CHAN = 4 - 1 + i;                                                               //CC1 connect to AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //start sampling
    while ((ADC_CTRL & bADC_IF) == 0);                                              //check finish flag
    ADC_CTRL = bADC_IF;                                                             //clear flag
    UCCn_Value = ADC_DAT & 0xFFF;
#elif defined(CH552)
    ADC_IF = 0;
    if (i == 1) {
      ADC_CHAN1 = 0;
      ADC_CHAN0 = 1;
    } else {
      ADC_CHAN1 = 1;
      ADC_CHAN0 = 0;
    }
    ADC_START = 1;
    while (ADC_IF == 0);
    UCCn_Value = ADC_DATA;
#endif
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
  //pinMode(17, OUTPUT);
  //P1_7 = 0; //!!!!!!
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

    setT0ForCC();
    status = ReceiveHandle();
    restoreT0ForTiming();

    if ( status == 0 ) {
      switch (Union_Header->HeaderStruct.MsgType) {
        case SourceSendCap:
          {
            __data uint8_t searchIndex = 0xFF;
            __data uint8_t voltageSettingCount = Union_Header->HeaderStruct.NDO;
            __xdata uint16_t matchCurrent = 0;
            __xdata uint16_t matchVoltage = 0;
            __xdata uint16_t voltageMatchError = 65535;
            Serial0_print("VA:");
            for (__data uint8_t i = 0; i < voltageSettingCount; i++) {
              Union_SrcCap = (__xdata _Union_SrcCap * __data)(&RcvDataBuf[2 + 4 * i]);
              if (Union_SrcCap->SrcCapFixedSupplyStruct.Fixedsupply == 0b00) {
                //Fixed supply
                //B9...0 Maximum Current in 10mA units
                __xdata uint16_t current = (Union_SrcCap->SrcCapFixedSupplyStruct.Current) * 10;
                //B19...10 Voltage in 50mV units
                __xdata uint16_t voltage = (((Union_SrcCap->SrcCapFixedSupplyStruct.VoltH4) << 6) | (Union_SrcCap->SrcCapFixedSupplyStruct.VoltL6)) * 50;
                Serial0_print(voltage);
                Serial0_print(",");
                Serial0_print(current);
                Serial0_print(";");

                if ((voltage >= ALLOWED_MIN_VOLT_MV) && (voltage <= ALLOWED_MAX_VOLT_MV)) {
                  uint16_t errorInThisVoltage = (uint16_t)abs(((int16_t)TARGET_VOLT_MV) - ((int16_t)voltage));
                  if (errorInThisVoltage < voltageMatchError) {
                    //find a voltage within range and closest to the ideal one
                    searchIndex = i;
                    matchVoltage = 0;
                    matchCurrent = current / 10;
                    voltageMatchError = errorInThisVoltage;
                  }
                }
              } else if (Union_SrcCap->SrcCapFixedSupplyStruct.Fixedsupply == 0b11) {
                if (Union_SrcCap->SrcCapAugmentedSupplyStruct.PPS == 0) {
                  //Augmented Power Data Object
                  __xdata uint16_t minVoltage = (Union_SrcCap->SrcCapAugmentedSupplyStruct.MinVoltage) * 100;
                  __xdata uint16_t maxVoltage = (((Union_SrcCap->SrcCapAugmentedSupplyStruct.MaxVoltageH1) << 7) | (Union_SrcCap->SrcCapAugmentedSupplyStruct.MaxVoltageL7)) * 100;
                  __xdata uint16_t current = (Union_SrcCap->SrcCapAugmentedSupplyStruct.MaxCurrent) * 50;
                  Serial0_print(minVoltage);
                  Serial0_print("~");
                  Serial0_print(maxVoltage);
                  Serial0_print(",");
                  Serial0_print(current);
                  Serial0_print(";");

                  //only do a precise match in PPS mode
                  if ((voltageMatchError>0) && (TARGET_VOLT_MV>=minVoltage) && (TARGET_VOLT_MV<=maxVoltage)){
                    matchVoltage = ((TARGET_VOLT_MV+10)/20);
                    matchCurrent = current / 50;
                    voltageMatchError = 0;  //do no more search
                    searchIndex = i;
                  }
                }
              }
            }
            Serial0_flush();
            if (searchIndex != 0xFF) {
              // prepare package to request high voltage
              ResetSndHeader();
              Union_Header->HeaderStruct.PortPwrRole = PwrRoleSink;
              Union_Header->HeaderStruct.PortDataRole = DataRoleUFP;
              Union_Header->HeaderStruct.NDO = 1;
              Union_Header->HeaderStruct.MsgType = SinkSendRequest;

              //clear data buffer
              for (uint8_t i = 0; i < 4; i++) {
                SndDataBuf[2 + i] = 0;
              }
              if (matchVoltage == 0) {
                //this is fixed. As fixed voltage request use obj id for voltage
                ((_Sink_Request_Data_Fixed_Struct *)(&SndDataBuf[2]))->MaxCurrent = matchCurrent;
                ((_Sink_Request_Data_Fixed_Struct *)(&SndDataBuf[2]))->CurrentL6 = matchCurrent & (0x3F);
                ((_Sink_Request_Data_Fixed_Struct *)(&SndDataBuf[2]))->CurrentH4 = (matchCurrent >> 6) & (0xF);
                ((_Sink_Request_Data_Fixed_Struct *)(&SndDataBuf[2]))->ObjectPosition = searchIndex + 1;
              }else{
                //this is pps
                ((_Sink_Request_Data_Programmable_Struct *)(&SndDataBuf[2]))->Current = matchCurrent;
                ((_Sink_Request_Data_Programmable_Struct *)(&SndDataBuf[2]))->VoltageL7 = matchVoltage & (0x7F);
                ((_Sink_Request_Data_Programmable_Struct *)(&SndDataBuf[2]))->VoltageH4 = (matchVoltage >> 7) & (0xF);
                ((_Sink_Request_Data_Programmable_Struct *)(&SndDataBuf[2]))->ObjectPosition = searchIndex + 1;
              }
              setT0ForCC();
              SendHandle();
              restoreT0ForTiming();
            } else {
              Serial0_println("No Matched Volt.");
            }
          }
          break;
        case Accept:
          Serial0_println("Accept");
          break;
        case PS_RDY:
          Serial0_println("Ready");
          break;
        case GetSinkCap:
          ResetSndHeader();
          Union_Header->HeaderStruct.PortPwrRole = PwrRoleSink;
          Union_Header->HeaderStruct.PortDataRole = DataRoleUFP;
          Union_Header->HeaderStruct.NDO = 1;
          Union_Header->HeaderStruct.MsgType = SinkCap;
          //6.4.1.3 Sink Capabilities Message
          //battery
          //Maximum Voltage in 50mV units: 585
          //Minimum Voltage in 50mV units: 0
          //Operational Power in 250mW units:288
          //not sure why WCH use this data
          SndDataBuf[2] = 0x20;
          SndDataBuf[3] = 0x01;
          SndDataBuf[4] = 0x90;
          SndDataBuf[5] = 0x64;
          SendHandle();
          break;
        case REJECT:
          Serial0_println("Reject");
          break;
        case SoftRst:
          Serial0_println("Soft Reset");
          break;
        default :
          Serial0_print("MsgType:");
          Serial0_println( (int)(Union_Header->HeaderStruct.MsgType));
          break;
      }

      //
    }

    P1_6 = 0;//!!!!!!

  }

}
