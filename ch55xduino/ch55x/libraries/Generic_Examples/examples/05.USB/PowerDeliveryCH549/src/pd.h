// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
// clang-format on

// Check power capability with ADC value from CC1/CC2
#define  DefaultPowerMin  (342-137)
#define  DefaultPowerMax  (342+158)
#define  Power1_5AMin     (771-198)
#define  Power1_5AMax     (771+180)
#define  Power3_0AMin     (1383-310)
#define  Power3_0AMax     (1383+288) 

// size of RcvDataBuf
#define  RcvBufMaxLen	   73
// size of SndDataBuf
#define  SndBufMaxLen	   73

// Header bit define
#define  DataRoleUFP       0
#define  DataRoleDFP       1
#define  PwrRoleSink       0
#define  PwrRoleSource     1

// Message Type : Data Message 
#define  SourceSendCap	   0x01		                                   // 5B:00001
#define  GoodCRC     	   0x01		                                   // 5B:00001
#define  SinkSendRequest   0x02                                        // 5B:00010
#define  Accept            0x03                                        // 5B:00011
#define  REJECT            0x04                                        // 5B:00100
#define  SinkCap           0x04                                        // 5B:00100
#define  PS_RDY  		   0x06		                                   // 5B:00110
#define  GetSourceCap      0x07                                        // 5B:00111
#define  GetSinkCap	       0x08		                                   // 5B:01000
#define  PRSwap            0x0A                                        // 5B:01010
#define  SoftRst		   0x0D		                                   // 5B:01101
#define  VDef		       0x0F

#define  VDef		       0x0F

// Vendor Define Message Command
#define  DiscIdent		   0x01
#define  DiscSVID		   0x02
#define  DiscMode		   0x03
#define  EnterMode		   0x04
#define  ExitMode		   0x05
#define  Attention		   0x06                                        //SINK请求通知，表示设备连接状态
#define  DPStatUpdate	   0x10
#define  DPConfig		   0x11

#define  REQ			   0x00
#define  ACK			   0x01

// other non-data 4B5B code
#define  Sync1		       0x18		                                   // 5B:11000
#define  Sync2		       0x11		                                   // 5B:10001
#define  RST1		       0x07		                                   // 5B:00111
#define  RST2		       0x19		                                   // 5B:11001
#define  EOP			   0x0D		                                   // 5B:01101
#define  Sync3		       0x06		                                   // 5B:00110

// Start of Packet Sequences
#define  SOP               0                                           //Start of Packet Sequence(SOP)
#define  SOPP              1                                           //Start of Packet Sequence Prime(SOP')
#define  SOPDP             2                                           //Start of Packet Sequence Double Prime(SOP'')
#define  SOPPD             3                                           //Start of Packet Sequence Prime Double(SOP'_Debug)
#define  SOPDPD            4                                           //Start of Packet Sequence Double Prime Double(SOP''_Debug)

// receive data status
#define  REVSUCCESS        0x00                                        //收到数据
#define  NODATA            0x01                                        //无数据
#define  ILLEGAL           0x02                                        //收到非法数据包，可能是SOP'

// connection status
#define  DFP_PD_CONNECT    0x00
#define  DFP_PD_DISCONNECT 0x01

// Header
typedef struct  {
//	Extended	  1		0
//	NDO			  3
//	MsgID		  3
//	PortPwrRole	  1		0:Sink  1:Source
//	SpecRev		  2		01 Rev2.0
//	PortDataRole  1		0:UFP  1:DFP
//	MessageType	  5

	uint8_t PortPwrRole:1;
	uint8_t MsgID:3;
	uint8_t NDO:3;
	uint8_t Extended:1;
	
	uint8_t MsgType:5;
	uint8_t PortDataRole:1;
	uint8_t SpecRev:2;
	
} _Msg_Header_Struct;

// header
typedef union {
	_Msg_Header_Struct  HeaderStruct;
	uint8_t HeaderData[2];
} _Union_Header;

// voltage current parsing
typedef struct  {
//	Data	12bit		
//	Volt	10bit		Voltage *0.05V
//	Curr	10bit		Current *0.01A

	uint16_t VoltH4:4;
	uint16_t Data:12;

	uint16_t  Current:10;
	uint16_t  VoltL6:6;

} _SRC_Cap_Struct;

typedef union {				                                              /* Src Cap */
	_SRC_Cap_Struct  SrcCapStruct;
	uint8_t SrcCapData[4];
} _Union_SrcCap;

// VDM Header
typedef struct  {
//	SVID			16		0xFF01: DisplayPort
//
//	StructuredVDM	1
//	SVDMVer			2		00: Ver.1
//	----			5
//	CommandType		2		00: REQ		01: ACK
//	----			1
//	Command			5
	
	uint16_t SVID:16;
	
	uint16_t Command:5;
	uint16_t :1;
	uint16_t CommandType:2;
	uint16_t ModeIndex:3;
	uint16_t :2;
	uint16_t SVDMVer:2;
	uint16_t StructuredVDM:1;
	
} _VDM_Hdr_Struct;
typedef union {
	_VDM_Hdr_Struct  VDMHdrStruct;
	uint8_t VDMHdrData[4];
} _Union_VDM_Hdr;

// external reference
// select cc pin: 1:cc1 2:cc2
extern __xdata uint8_t CCSel;
// current SOP type received
extern __xdata uint8_t RecvSop;
// pointer to header
//extern _Union_Header xdata *Union_Header;
// pointer to voltage current pair structure
//extern _Union_SrcCap xdata *Union_SrcCap;
// VDM header pointer
//extern _Union_VDM_Hdr xdata *Union_VDM_Hdr;

extern uint8_t RcvDataBuf[];
extern uint8_t RcvDataCount;

extern uint8_t SndDataBuf[];
extern uint8_t SndDataCount;

extern __code uint8_t Cvt5B4B[];
extern __code uint8_t Cvt4B5B[];
extern __code uint32_t CRC32_Table[];

// initialize sending header (PD2.0 version, MsgID initialized，PortDataRole,PortPwrRole customizable)
void  ResetSndHeader ();
//send content of SndDataBuf
uint8_t SendHandle ();
//check receving，0x00:packet received；0x01:packet not received；0x02:packet not valid. Data stored in RcvDataBuf
uint8_t ReceiveHandle () ;                           
