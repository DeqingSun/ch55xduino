#include "USBSCSI.h"

__code const uint8_t Page00_Inquiry_Data[] = {
    0x00,                  /* PERIPHERAL QUALIFIER & PERIPHERAL DEVICE TYPE*/
    0x00, 0x00, 0x00, 0x00 /* Supported Pages 00*/
};

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
__code const uint8_t Mode_Sense6_data[] = {
    0x03,
    0x00,
    0x00, // refer to SBC-3, Table 97 - DEVICE-SPECIFIC PARAMETER field for
          // direct-access block devices, 0x80 is WP (Whole disk is read only).
          // 0x00 is normal
    0x00,
};

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

__code const uint8_t Mode_Sense10_data[] = {0x00, 0x06, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00};

__code const uint8_t Scsi_Sense_Data[] = {
    0x70,                       /*RespCode*/
    0x00,                       /*SegmentNumber*/
    NO_SENSE,                   /* Sens_Key*/
    0x00,     0x00, 0x00, 0x00, /*Information*/
    0x0A,                       /*AdditionalSenseLength*/
    0x00,     0x00, 0x00, 0x00, /*CmdInformation*/
    NO_SENSE,                   /*Asc*/
    0x00,                       /*ASCQ*/
    0x00,                       /*FRUC*/
    0x00,                       /*TBD*/
    0x00,     0x00              /*SenseKeySpecific*/
};
