#include "USBconstant.h"

// Device descriptor
__code uint8_t DevDesc[] = {
    0x12,
    0x01,
    0x10,
    0x01, // USB spec release number in BCD format, USB1.1 (0x10, 0x01).
    0xEF,
    0x02,
    0x01,               // bDeviceClass, bDeviceSubClass, bDeviceProtocol
    DEFAULT_ENDP0_SIZE, // bNumConfigurations
    0x81,
    0x17,
    0x9F,
    0x0C, // VID PID
    0x04,
    0x01, // version
    0x01,
    0x02,
    0x03, // bString
    0x01  // bNumConfigurations
};

__code uint16_t DevDescLen = sizeof(DevDesc);

__code uint8_t CfgDesc[] = {
    0x09, 0x02, sizeof(CfgDesc) & 0xff, sizeof(CfgDesc) >> 8, 0x03, 0x01, 0x00,
    0x80, 0x64, // Configuration descriptor (3 interfaces)

    // Interface 0 (tinyspi) descriptor
    0x09, 0x04, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00,
    0x04, // tinyspi description, 1 endpoint
    0x07, 0x05, 0x81, 0x03, 0x08, 0x00,
    0x64, // endpoint descriptor, max packet 8 bytes

    // Interface Association Descriptor, IAD, this packes following 2 interfaces
    // into 1
    0x08, 0x0B, 0x01, 0x02, 0x02, 0x02, 0x01,
    0x05, // first interface 1, interface count 2, function class 2, function
          // subclass 2, interface protocol 1, Function String 5
    // Interface 1 (CDC) descriptor
    0x09, 0x04, 0x01, 0x00, 0x01, 0x02, 0x02, 0x01,
    0x05, // CDC control description, 1 endpoint
    // Functional Descriptor refer to usbcdc11.pdf
    0x05, 0x24, 0x00, 0x10, 0x01, // Header Functional Descriptor
    0x05, 0x24, 0x01, 0x00, 0x00, // Call Management Functional Descriptor
    0x04, 0x24, 0x02, 0x02, // Direct Line Management Functional Descriptor,
                            // Support: Set_Line_Coding, Set_Control_Line_State,
                            // Get_Line_Coding, Serial_State
    0x05, 0x24, 0x06, 0x01, 0x02, // Union Functional Descriptor, Communication
                                  // class interface 1, Data Class Interface 2
    0x07, 0x05, 0x82, 0x03, 0x08, 0x00,
    0x40, // EndPoint descriptor (CDC Upload, Interrupt)
    // Interface 2 (Data Interface) descriptor
    0x09, 0x04, 0x02, 0x00, 0x02, 0x0a, 0x00, 0x00,
    0x05,                                     // Data Class Interface descriptor
    0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00, // endpoint descriptor
    0x07, 0x05, 0x83, 0x02, 0x40, 0x00, 0x00, // endpoint descriptor

};

__code uint16_t CfgDescLen = sizeof(CfgDesc);

// String Descriptors
__code uint8_t LangDes[] = {0x04, 0x03, 0x09, 0x04}; // Language Descriptor
__code uint16_t LangDesLen = sizeof(LangDes);
__code uint8_t SerDes[] = {
    // Serial String Descriptor
    0x10, 0x03, 't', 0x00, 'i', 0x00, 'n', 0x00,
    'y',  0x00, 'I', 0x00, 'S', 0x00, 'P', 0x00,
};
__code uint16_t SerDesLen = sizeof(SerDes);
__code uint8_t Prod_Des[] = { // Produce String Descriptor
    0x16, 0x03, 'U',  0x00, 'S',  0x00, 'B',  0x00, 't',  0x00, 'i',
    0x00, 'n',  0x00, 'y',  0x00, 'S',  0x00, 'P',  0x00, 'I',  0x00};
__code uint16_t Prod_DesLen = sizeof(Prod_Des);

__code uint8_t Debugger_Des[] = {
    // Debugger String Descriptor
    0x1C, 0x03, 'T', 0x00, 'i', 0x00, 'n', 0x00, 'y', 0x00,
    'S',  0x00, 'P', 0x00, 'I', 0x00, ' ', 0x00, 'D', 0x00,
    'W',  0x00, 'i', 0x00, 'r', 0x00, 'e', 0x00,
};
__code uint16_t Debugger_DesLen = sizeof(Debugger_Des);

__code uint8_t CDC_Des[] = {0x16, 0x03, 'C', 0x00, 'D', 0x00, 'C', 0x00,
                            ' ',  0x00, 'S', 0x00, 'e', 0x00, 'r', 0x00,
                            'i',  0x00, 'a', 0x00, 'l', 0x00};
__code uint16_t CDC_DesLen = sizeof(CDC_Des);

__code uint8_t Manuf_Des[] = {
    0x0E, 0x03, 'D',  0x00, 'e',  0x00, 'q',
    0x00, 'i',  0x00, 'n',  0x00, 'g',  0x00,
};
__code uint16_t Manuf_DesLen = sizeof(Manuf_Des);
