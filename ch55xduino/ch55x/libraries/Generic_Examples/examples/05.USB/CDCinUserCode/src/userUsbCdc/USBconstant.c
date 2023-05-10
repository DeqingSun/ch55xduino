#include "USBconstant.h"


//Device descriptor
__code uint8_t DevDesc[] = {
    0x12,0x01,
    0x10,0x01,  //USB spec release number in BCD format, USB1.1 (0x10, 0x01).
    0xEF,0x02,0x01, //bDeviceClass, bDeviceSubClass, bDeviceProtocol 
    DEFAULT_ENDP0_SIZE, //bNumConfigurations
    0x09,0x12,0x50,0xC5, // VID PID 
    0x01,0x01,  //version
    0x01,0x02,0x03, //bString
    0x01    //bNumConfigurations
};

__code uint16_t DevDescLen = sizeof(DevDesc);

__code uint8_t CfgDesc[] ={
    0x09,0x02,sizeof(CfgDesc) & 0xff,sizeof(CfgDesc) >> 8,
    0x02,0x01,0x00,0x80,0x64,             //Configuration descriptor (2 interfaces)
    // Interface Association Descriptor, IAD, this packes following 2 interfaces into 1
    0x08,0x0B,0x00,0x02,0x02,0x02,0x01,0x04,
    // Interface 1 (CDC) descriptor
    0x09,0x04,0x00,0x00,0x01,0x02,0x02,0x01,0x04,    // CDC control description, 1 endpoint
    // Functional Descriptor refer to usbcdc11.pdf
    0x05,0x24,0x00,0x10,0x01,                                 //Header Functional Descriptor
    0x05,0x24,0x01,0x00,0x00,                                 //Call Management Functional Descriptor
    0x04,0x24,0x02,0x02,                                      //Direct Line Management Functional Descriptor, Support: Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, Serial_State 
    0x05,0x24,0x06,0x00,0x01,                                 //Union Functional Descriptor, Communication class interface 0, Data Class Interface 1
    0x07,0x05,0x81,0x03,0x08,0x00,0x40,                       //EndPoint descriptor (CDC Upload, Interrupt)
    // Interface 2 (Data Interface) descriptor
    0x09,0x04,0x01,0x00,0x02,0x0a,0x00,0x00,0x04,             //Data Class Interface descriptor
    0x07,0x05,0x02,0x02,0x40,0x00,0x00,                       //endpoint descriptor
    0x07,0x05,0x82,0x02,0x40,0x00,0x00,                       //endpoint descriptor

};

__code uint16_t CfgDescLen = sizeof(CfgDesc);

//String Descriptors
__code uint8_t LangDes[]={0x04,0x03,0x09,0x04};           //Language Descriptor
__code uint16_t LangDesLen = sizeof(LangDes);
__code uint16_t SerDes[]={                                 //Serial String Descriptor
    0x030C,
    'C','H','5','5','x',
};
__code uint16_t SerDesLen = sizeof(SerDes);
__code uint16_t Prod_Des[]={                                //Produce String Descriptor
    0x0316,
    'C','H','5','5','x','d','u','i','n','o',
};
__code uint16_t Prod_DesLen = sizeof(Prod_Des);

__code uint16_t CDC_Des[]={
    0x0316,
    'C','D','C',' ','S','e','r','i','a','l',
};
__code uint16_t CDC_DesLen = sizeof(CDC_Des);

__code uint16_t Manuf_Des[]={    //SDCC is little endian
    0x030E,
    'D','e','q','i','n','g',
};
__code uint16_t Manuf_DesLen = sizeof(Manuf_Des);
