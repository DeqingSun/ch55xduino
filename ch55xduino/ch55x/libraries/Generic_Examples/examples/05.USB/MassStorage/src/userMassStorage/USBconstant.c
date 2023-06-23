#include "USBconstant.h"

// Device descriptor
__code USB_Descriptor_Device_t DeviceDescriptor = {
    .Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification = VERSION_BCD(1, 1, 0),
    .Class = 0x00,
    .SubClass = 0x00,
    .Protocol = 0x00,

    .Endpoint0Size = DEFAULT_ENDP0_SIZE,

    .VendorID = 0x1209,
    .ProductID = 0xc55e,
    .ReleaseNumber = VERSION_BCD(1, 0, 1),

    .ManufacturerStrIndex = 1,
    .ProductStrIndex = 2,
    .SerialNumStrIndex = 3,

    .NumberOfConfigurations = 1};

/** Configuration descriptor structure. This descriptor, located in FLASH
 * memory, describes the usage of the device in one of its supported
 * configurations, including information about any device interfaces and
 * endpoints. The descriptor is read out by the USB host during the enumeration
 * process when selecting a configuration so that the host may correctly
 * communicate with the USB device.
 */
__code USB_Descriptor_Configuration_t ConfigurationDescriptor = {
    .Config = {.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t),
                          .Type = DTYPE_Configuration},

               .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
               .TotalInterfaces = 1,

               .ConfigurationNumber = 1,
               .ConfigurationStrIndex = NO_DESCRIPTOR,

               .ConfigAttributes = USB_CONFIG_ATTR_RESERVED,

               .MaxPowerConsumption = USB_CONFIG_POWER_MA(100)},

    .MS_Interface = {.Header = {.Size = sizeof(USB_Descriptor_Interface_t),
                                .Type = DTYPE_Interface},

                     .InterfaceNumber = 0,
                     .AlternateSetting = 0,

                     .TotalEndpoints = 2,

                     .Class = MS_CSCP_MassStorageClass,
                     .SubClass = MS_CSCP_SCSITransparentSubclass,
                     .Protocol = MS_CSCP_BulkOnlyTransportProtocol,

                     .InterfaceStrIndex = NO_DESCRIPTOR},

    .MS_DataInEndpoint = {.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                     .Type = DTYPE_Endpoint},

                          .EndpointAddress = MASS_STORAGE_IN_EPADDR,
                          .Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC |
                                         ENDPOINT_USAGE_DATA),
                          .EndpointSize = MASS_STORAGE_IO_EPSIZE,
                          .PollingIntervalMS = 0x01},

    .MS_DataOutEndpoint = {.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
                                      .Type = DTYPE_Endpoint},

                           .EndpointAddress = MASS_STORAGE_OUT_EPADDR,
                           .Attributes = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC |
                                          ENDPOINT_USAGE_DATA),
                           .EndpointSize = MASS_STORAGE_IO_EPSIZE,
                           .PollingIntervalMS = 0x01}};

// String Descriptors
__code uint8_t LanguageDescriptor[] = {0x04, 0x03, 0x09,
                                       0x04}; // Language Descriptor
__code uint16_t SerialDescriptor[] = {
    // Serial String Descriptor
    (((5 + 1) * 2) | (DTYPE_String << 8)), 'C', 'H', '5', '5', 'x',
};
__code uint16_t ProductDescriptor[] = {
    // Produce String Descriptor
    (((14 + 1) * 2) | (DTYPE_String << 8)),
    'C',
    'H',
    '5',
    '5',
    'x',
    'd',
    'u',
    'i',
    'n',
    'o',
    ' ',
    'M',
    'S',
    'D',
};
__code uint16_t ManufacturerDescriptor[] = {
    // SDCC is little endian
    (((6 + 1) * 2) | (DTYPE_String << 8)), 'D', 'e', 'q', 'i', 'n', 'g',
};
