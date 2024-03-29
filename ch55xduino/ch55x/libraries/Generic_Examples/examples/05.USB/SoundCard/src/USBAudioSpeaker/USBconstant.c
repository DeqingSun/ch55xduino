#include "USBconstant.h"


//Device descriptor
__code uint8_t DevDesc[] = {
	0x12,	//Descriptor size is 18 bytes  
	0x01,  	//DEVICE Descriptor Type  
	0x00,0x02,  //USB Specification version 2.00  
	0x00,  	//Each interface specifies its own class information  
	0x00,  	//Each interface specifies its own Subclass information  
	0x00,  	//No protocols the device basis  
	DEFAULT_ENDP0_SIZE,  	//Maximum packet size for endpoint zero is 8  
	0x09,0x12,  //Vendor ID 
	0x5A,0xC5,  //Product ID
	0x01,0x00,  //The device release number is 0.01  
	0x01,  	//The manufacturer string descriptor index is 1  
	0x02,  	//The product string descriptor index is 2  
	0x00,  	//The device doesn't have the string descriptor describing the serial number  
	0x01,  	//The device has 1 possible configurations  
};

__code uint16_t DevDescLen = sizeof(DevDesc);

__code uint8_t CfgDesc[] ={
  0x09,  	 //Descriptor size is 9 bytes  
  0x02,  	 //CONFIGURATION Descriptor Type  
  sizeof(CfgDesc) & 0xff,sizeof(CfgDesc) >> 8, //The total length of data for this configuration
  0x02,  	 //This configuration supports 2 interfaces  
  0x01,  	 //The value 1 should be used to select this configuration  
  0x00,  	 //The device doesn't have the string descriptor describing this configuration  
  0x80,  	 //Configuration characteristics : Bit 7: Reserved (set to one) 1 Bit 6: Self-powered 1 Bit 5: Remote Wakeup 0  
  0x32,  	 //Maximum power consumption of the device in this configuration is 100 mA  

  0x09,  	 //Descriptor size is 9 bytes  
  0x04,  	 //INTERFACE Descriptor Type  
  0x00,  	 //The number of this interface is 0.  
  0x00,  	 //The value used to select the alternate setting for this interface is 0  
  0x00,  	 //The number of endpoints used by this interface is 0 (excluding endpoint zero)  
  0x01,  	 //The interface implements the Audio Interface class  
  0x01,  	 //The interface implements the AUDIOCONTROL Subclass  
  0x00,  	 //The interface uses the IP_VERSION_01_00 Protocol  
  0x00,  	 //The device doesn't have a string descriptor describing this iInterface  

  0x09,  	 //Size of the descriptor, in bytes  
  0x24,  	 //CS_INTERFACE Descriptor Type  
  0x01,  	 //HEADER descriptor subtype  
  0x00,0x01,  	 //Audio Device compliant to the USB Audio specification version 1.00  
  0x1E,0x00,  	 //Total number of bytes returned for the class-specific AudioControl interface descriptor. Includes the combined length of this descriptor header and all Unit and Terminal descriptors.  
  0x01,  	 //The number of AudioStreaming and MIDIStreaming interfaces in the Audio Interface Collection to which this AudioControl interface belongs  
  0x01,  	 //Interface number of the AudioStreaming or MIDIStreaming interface in the Collection  

  0x0C,  	 //Size of the descriptor, in bytes  
  0x24,  	 //CS_INTERFACE Descriptor Type  
  0x02,  	 //INPUT_TERMINAL descriptor subtype  
  0x01,  	 //Constant uniquely identifying the Terminal within the audio function. This value is used in all requests to address this Terminal.  
  0x01,0x01, //A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  
  0x00,  	 //This Input Terminal has no association  
  0x01,  	 //This Terminal's output audio channel cluster has 1 logical output channels  
  0x04,0x00, //Spatial locations present in the cluster
			 //Bit 0: Left Front 1
			 //Bit 1: Right Front 1
			 //Bit 2: Center Front 0
			 //Bit 3: Low Freq Enh 0
			 //Bit 4: Left Surround 0
			 //Bit 5: Right Surround 0
			 //Bit 6: Left of Center 0
			 //Bit 7: Right of Center 0
			 //Bit 8: Surround 0
			 //Bit 9: ...  
  0x00,  	//Index of a string descriptor, describing the name of the first logical channel.  
  0x00,  	//Index of a string descriptor, describing the Input Terminal.  
			
  0x09,  	//Size of the descriptor, in bytes  
  0x24,  	//CS_INTERFACE Descriptor Type  
  0x03,  	//OUTPUT_TERMINAL descriptor subtype  
  0x02,  	//Constant uniquely identifying the Terminal within the audio function. This value is used in all requests to address this Terminal.  
  0x01,0x03,//A generic speaker or set of speakers that does not fit under any of the other classifications.  
  0x00,  	//This Output Terminal has no association  
  0x01,  	//ID of the Unit or Terminal to which this Terminal is connected.  
  0x00,  	//Index of a string descriptor, describing the Output Terminal.  

  0x09,  	//Descriptor size is 9 bytes  
  0x04,  	//INTERFACE Descriptor Type  
  0x01,  	//The number of this interface is 1.  
  0x00,  	//The value used to select the alternate setting for this interface is 0  
  0x00,  	//The number of endpoints used by this interface is 0 (excluding endpoint zero)  
  0x01,  	//The interface implements the Audio Interface class  
  0x02,  	//The interface implements the AUDIOSTREAMING Subclass  
  0x00,  	//The interface uses the IP_VERSION_01_00 Protocol  
  0x00,  	//The device doesn't have a string descriptor describing this iInterface  

  0x09,  	//Descriptor size is 9 bytes  
  0x04,  	//INTERFACE Descriptor Type  
  0x01,  	//The number of this interface is 1.  
  0x01,  	//The value used to select the alternate setting for this interface is 1  
  0x01,  	//The number of endpoints used by this interface is 1 (excluding endpoint zero)  
  0x01,  	//The interface implements the Audio Interface class  
  0x02,  	//The interface implements the AUDIOSTREAMING Subclass  
  0x00,  	//The interface uses the IP_VERSION_01_00 Protocol  
  0x00,  	//The device doesn't have a string descriptor describing this iInterface  

  0x07,  	//Size of the descriptor, in bytes  
  0x24,  	//CS_INTERFACE Descriptor Type  
  0x01,  	//AS_GENERAL descriptor subtype  
  0x01,  	//The Terminal ID of the Terminal to which the endpoint of this interface is connected.  
  0x01,  	//Delay introduced by the data path. Expressed in number of frames.  
  0x01,0x00,//PCM. It seems PCM8 is not supported.

  0x0B,  	//Size of the descriptor, in bytes  
  0x24,  	//CS_INTERFACE Descriptor Type  
  0x02,  	//FORMAT_TYPE descriptor subtype  
  0x01,  	//FORMAT_TYPE_I  
  0x01,  	//Indicates the number of physical channels in the audio data stream.  
  0x02,  	//The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.  
  0x10,  	//The number of effectively used bits from the available bits in an audio subframe.  
  0x01,  	//Indicates how the sampling frequency can be programmed:  
  0xC0,0x5D,0x00, //	  Sampling frequency 24000 in Hz for this isochronous data endpoint.  
  

  0x09,  	// Descriptor size is 9 bytes  
  0x05,  	// ENDPOINT Descriptor Type  
  0x01,  	// This is an OUT endpoint with endpoint number 1  
  0x0D,  	// Types - 
			//	Transfer: ISOCHRONOUS 
			//	Sync: Sync 
			//	Usage: Data EP  
  0x40,0x00, //Maximum packet size for this endpoint is 64 Bytes. If Hi-Speed, 0 additional transactions per frame  
  0x01,  	 //The polling interval value is every 1 Frames. If Hi-Speed, 1 uFrames/NAK  
  0x00,  	 //Refresh Rate 2**n ms where n = 0  
  0x00,  	 //Synchronization Endpoint (if used) is endpoint 0  
	
  0x07,  	 //Size of the descriptor, in bytes  
  0x25,  	 //CS_ENDPOINT Descriptor Type  
  0x01,  	 //AUDIO_EP_GENERAL descriptor subtype  
  0x00,  	 //Bit 0: Sampling Frequency 0
			 //Bit 1: Pitch 0
			 //Bit 7: MaxPacketsOnly 0  
  0x00,  	 //Indicates the units used for the wLockDelay field: 0: Undefined  
  0x00,0x00  //Indicates the time it takes this endpoint to reliably lock its internal clock recovery circuitry. Units used depend on the value of the bLockDelayUnits field.  

};

__code uint16_t CfgDescLen = sizeof(CfgDesc);

//String Descriptors
__code uint8_t LangDes[]={0x04,0x03,0x09,0x04};           //Language Descriptor
__code uint16_t LangDesLen = sizeof(LangDes);
__code uint8_t SerDes[]={                                 //Serial String Descriptor
    0x0C,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00
};
__code uint16_t SerDesLen = sizeof(SerDes);
__code uint8_t Prod_Des[]={                                //Produce String Descriptor
    0x22,0x03,
    'C',0x00,'H',0x00,'5',0x00,'5',0x00,'x',0x00,'d',0x00,
    'u',0x00,'i',0x00,'n',0x00,'o',0x00,
    ' ',0x00,'A',0x00,'u',0x00,'d',0x00,'i',0x00,'o',0x00
};
__code uint16_t Prod_DesLen = sizeof(Prod_Des);

__code uint8_t Manuf_Des[]={
    0x0E,0x03,
    'D',0x00,'e',0x00,'q',0x00,'i',0x00,'n',0x00,'g',0x00,
};
__code uint16_t Manuf_DesLen = sizeof(Manuf_Des);
