#ifndef __CONST_DATA_H__
#define __CONST_DATA_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdDescriptors.h"
#include "usbCommonDescriptors/MassStorageClassCommon.h"
// clang-format on

#define EP0_ADDR 0
#define EP1_ADDR 10

#define MASS_STORAGE_IN_EPADDR 0x81
#define MASS_STORAGE_OUT_EPADDR 0x01
#define MASS_STORAGE_IO_EPSIZE 64

/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
typedef struct {
  USB_Descriptor_Configuration_Header_t Config;

  // Mass Storage Interface
  USB_Descriptor_Interface_t MS_Interface;
  USB_Descriptor_Endpoint_t MS_DataInEndpoint;
  USB_Descriptor_Endpoint_t MS_DataOutEndpoint;
} USB_Descriptor_Configuration_t;

extern __code USB_Descriptor_Device_t DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t ReportDescriptor[];
extern __code uint8_t LanguageDescriptor[];
extern __code uint16_t SerialDescriptor[];
extern __code uint16_t ProductDescriptor[];
extern __code uint16_t ManufacturerDescriptor[];

#endif
