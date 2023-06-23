#ifndef __CONST_DATA_H__
#define __CONST_DATA_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdDescriptors.h"
#include "usbCommonDescriptors/CDCClassCommon.h"
// clang-format on

#define SET_LINE_CODING                                                        \
  0X20 // Configures DTE rate, stop-bits, parity, and number-of-character
#define GET_LINE_CODING                                                        \
  0X21 // This request allows the host to find out the currently configured line
       // coding.
#define SET_CONTROL_LINE_STATE                                                 \
  0X22 // This request generates RS-232/V.24 style control signals.

#define CDC_NOTIFICATION_EPADDR 0x81
#define CDC_NOTIFICATION_EPSIZE 0x08
#define CDC_TX_EPADDR 0x82
#define CDC_RX_EPADDR 0x02
#define CDC_TXRX_EPSIZE 0x40

/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
typedef struct {
  USB_Descriptor_Configuration_Header_t Config;
  USB_Descriptor_Interface_Association_t CDC_IAD;

  // CDC Command Interface
  USB_Descriptor_Interface_t CDC_CCI_Interface;
  USB_CDC_Descriptor_FunctionalHeader_t CDC_Functional_Header;
  USB_CDC_Descriptor_FunctionalACM_t CDC_Functional_ACM;
  USB_CDC_Descriptor_FunctionalUnion_t CDC_Functional_Union;
  USB_Descriptor_Endpoint_t CDC_NotificationEndpoint;

  // CDC Data Interface
  USB_Descriptor_Interface_t CDC_DCI_Interface;
  USB_Descriptor_Endpoint_t CDC_DataOutEndpoint;
  USB_Descriptor_Endpoint_t CDC_DataInEndpoint;
} USB_Descriptor_Configuration_t;

/** Enum for the device interface descriptor IDs within the device. Each
 * interface descriptor should have a unique ID index associated with it, which
 * can be used to refer to the interface from other descriptors.
 */
enum InterfaceDescriptors_t {
  INTERFACE_ID_CDC_CCI = 0, /**< CDC CCI interface descriptor ID */
  INTERFACE_ID_CDC_DCI = 1, /**< CDC DCI interface descriptor ID */
};

extern __code USB_Descriptor_Device_t DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t LanguageDescriptor[];
extern __code uint16_t SerialDescriptor[];
extern __code uint16_t ProductDescriptor[];
extern __code uint16_t CDCDescriptor[];
extern __code uint16_t ManufacturerDescriptor[];

#endif
