#include "pd.h"

//!!!!
#include <Arduino.h>
//!!!!

#if F_CPU == 32000000
#define TIMEOUTCOUNT_LIMIT 700
#define TL0_RECV_START_VALUE 0
#define TL0_RECV_BIT0_UPPER_LIMIT 121
#define TL0_RECV_BIT1_UPPER_LIMIT 87
#define TL0_RECV_BIT1_LOWER_LIMIT 50
#define TL0_SEND_START_VALUE 150
#define TL0_SEND_BIT1_TOGGLE 196
#elif F_CPU == 24000000
#define TIMEOUTCOUNT_LIMIT 525
#define TL0_RECV_START_VALUE 64
#define TL0_RECV_BIT0_UPPER_LIMIT 226
//TL0_RECV_BIT1_UPPER_LIMIT is only used in sync 1010..
//seems we need to make it more strict as loop take longer
#define TL0_RECV_BIT1_UPPER_LIMIT (129-8)
#define TL0_RECV_BIT1_LOWER_LIMIT 101
#define TL0_SEND_START_VALUE 176
#define TL0_SEND_BIT1_TOGGLE 211
#else
#error "This only run for 32M clock"
#endif

#define STR_INDIR(x) #x
#define STR(x) STR_INDIR(x)

__xdata uint8_t CCSel;
__xdata uint8_t RecvSop;
//extern _Union_Header xdata *Union_Header;
//extern _Union_SrcCap xdata *Union_SrcCap;
//extern _Union_VDM_Hdr xdata *Union_VDM_Hdr;

__xdata uint8_t RcvDataBuf[RcvBufMaxLen];
__xdata uint8_t RcvDataCount;
__xdata uint8_t SndDataBuf[SndBufMaxLen];
__xdata uint8_t SndDataCount;

__xdata uint8_t RcvMsgID;
__xdata uint8_t SndMsgID;
__xdata uint8_t SendingGoodCRCFlag;
__xdata uint8_t SndDone;

///pointer physically in data ram pointing to xdata
__xdata _Union_Header * __data Union_Header;
__xdata _Union_SrcCap * __data Union_SrcCap;

__bit pd_send_recv_flag;
__bit pd_send_recv_status;

__code uint8_t Cvt5B4B[32]={
    0xff,0xff,0xff,0xff,0xff,0x02,0xff,0x0e,
    0xff,0x08,0x04,0x0c,0xff,0x0a,0x06,0x00,
    0xff,0xff,0x01,0xff,0xff,0x03,0xff,0x0f,
    0xff,0x09,0x05,0x0d,0xff,0x0b,0x07,0xff
};

__code uint8_t Cvt4B5B[16]={
    0x1e,0x09,0x14,0x15,
    0x0a,0x0b,0x0e,0x0f,
    0x12,0x13,0x16,0x17,
    0x1a,0x1b,0x1c,0x1d
};

__code uint32_t CRC32_Table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};


#pragma callee_saves sendCharDebug
void sendCharDebug17(__data char c) // 8Mbps under 24M clk
{
  c; // avoid unreferenced function argument warning
  // uint8_t interruptOn = EA;
  // EA = 0;
  __asm__("  mov c,_EA         \n"
          "  clr a             \n"
          "  rlc a             \n"
          "  mov b,a           \n"
          "  clr _EA           \n");

  // using P1.4
  __asm__( // any branch will cause unpredictable timing due to code alignment
      "  mov a,dpl         \n" // the parameter of func

      "  clr c             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  rrc a             \n"
      "  mov _P1_7,c       \n"
      "  setb c            \n"
      "  mov _P1_7,c       \n");
  // if (interruptOn) EA = 1;

  __asm__("  mov a,b           \n"
          "  jz skipSetEADebug$\n"
          "  setb _EA          \n"
          "skipSetEADebug$:    \n");


          
}

uint32_t CalculateCRC(uint32_t dataPtrAndLen){
  //'dpl' (LSB),'dph','b' & 'acc'
  //DPTR is the array address, B is the low byte of length
  //https://web.mit.edu/freebsd/head/sys/libkern/crc32.c
  // __xdata uint8_t * __data dataPtr = (__xdata uint8_t * __data)(dataPtrAndLen&0xFFFF);
  // __data uint8_t len = (dataPtrAndLen>>16)&0xFF;

  // __data uint32_t crc;

  //   crc = 0xFFFFFFFF;
  //   while (len--)
  //     crc = CRC32_Table[(crc ^ (*dataPtr++)) & 0xFF] ^ (crc >> 8);
  //   crc = crc ^ 0xffffffff;

  // return crc;

  //sdcc is small-endian, so lowest byte is first in memory

  //B = CRC32_Table[(dataPtrAndLen&0xff)*4];
  dataPtrAndLen;
  __asm__(
    "; put data ptr to DPTR1                      \n"
    "    mov r0,dpl                               \n"
    "    mov r1,dph                               \n"
    "    inc _XBUS_AUX                            \n"
    "    mov dpl,r0                               \n"
    "    mov dph,r1                               \n"
    "    dec _XBUS_AUX                            \n"
    "    mov r7,b                                 \n"
    "; init crc (r0~r3,r0 lsb)                    \n"
    "    mov a,#0xff                              \n"
    "    mov r0,a                                 \n"
    "    mov r1,a                                 \n"
    "    mov r2,a                                 \n"
    "    mov r3,a                                 \n"

    "loop_crc_calc$:                              \n"
    "; prepare r5,r6 as LUT offset                \n"
    "    clr a                                    \n"
    "    mov r6,a                                 \n"
    "    mov r5,a                                 \n"
    "    inc _XBUS_AUX                            \n"
    "    movx a,@dptr                             \n"
    "    inc dptr                                 \n"
    "    dec _XBUS_AUX                            \n"
    "    xrl a,r0                                 \n"
    "; now a has the index of CRC table           \n"
    "    clr c                                    \n"
    "    rlc a                                    \n"
    "    mov r5,a                                 \n"
    "    mov a,r6                                 \n"
    "    rlc a                                    \n"
    "    mov r6,a                                 \n"
    "    clr c                                    \n"
    "    mov a,r5                                 \n"
    "    rlc a                                    \n"
    "    mov r5,a                                 \n"
    "    mov a,r6                                 \n"
    "    rlc a                                    \n"
    "    mov r6,a                                 \n"
    "; now a has the offser of CRC table          \n"
    "    mov dptr,#(_CRC32_Table)                 \n"
    "    mov a,dpl                                \n"
    "    add a,r5                                 \n"
    "    mov dpl,a                                \n"
    "    mov a,dph                                \n"
    "    addc a,r6                                \n"
    "    mov dph,a                                \n"
    "    clr a                                    \n"
    "    movc a,@a+dptr                           \n"
    "    xrl a,r1                                 \n"
    "    mov r0,a                                 \n"
    "    mov a,#1                                 \n"
    "    movc a,@a+dptr                           \n"
    "    xrl a,r2                                 \n"
    "    mov r1,a                                 \n"
    "    mov a,#2                                 \n"
    "    movc a,@a+dptr                           \n"
    "    xrl a,r3                                 \n"
    "    mov r2,a                                 \n"
    "    mov a,#3                                 \n"
    "    movc a,@a+dptr                           \n"
    "    mov r3,a                                 \n"
    "    djnz r7,loop_crc_calc$             \n"
    " ; now r0~r3 has the CRC result, do an inv   \n"
    "    mov a,#0xFF                              \n"
    "    xrl a,r0                                 \n"
    "    mov dpl,a                                \n"
    "    mov a,#0xFF                               \n"
    "    xrl a,r1                                 \n"
    "    mov dph,a                                \n"
    "    mov a,#0xFF                               \n"
    "    xrl a,r2                                 \n"
    "    mov b,a                                  \n"
    "    mov a,#0xFF                               \n"
    "    xrl a,r3                                 \n"
  );

}

void ResetSndHeader() {
  Union_Header = (__xdata _Union_Header * __data)SndDataBuf;
  Union_Header->HeaderData[0] = 0;
  Union_Header->HeaderData[1] = 0;
  Union_Header->HeaderStruct.SpecRev = 2;
  Union_Header->HeaderStruct.MsgID = SndMsgID;
  //by default, NDO is 0
}

void RcvBufDecode5B4B(){
  //now we deal with header
  //the SOP is no longer needed, reuse the space for decoded data
  
  RcvDataBuf[0] = Cvt5B4B[RcvDataBuf[4]] + (Cvt5B4B[RcvDataBuf[5]] << 4);
  RcvDataBuf[1] = Cvt5B4B[RcvDataBuf[6]] + (Cvt5B4B[RcvDataBuf[7]] << 4);

  //something different from the original code
  //covert all received data from 5b to 4b
  //NDO is the number of data objects, each data object is 4 bytes, and 4 byte CRC. each byte is 2 5b
  __data uint8_t leftOverData = (((_Msg_Header_Struct *)(RcvDataBuf))->NDO*4+4)*2; 
  for (__data uint8_t i = 0; i < leftOverData; i++) {
    //header is already converted
    RcvDataBuf[2*i+2] = Cvt5B4B[RcvDataBuf[8+i*4+0]] + (Cvt5B4B[RcvDataBuf[8+i*4+1]] << 4);
    RcvDataBuf[2*i+3] = Cvt5B4B[RcvDataBuf[8+i*4+2]] + (Cvt5B4B[RcvDataBuf[8+i*4+3]] << 4);
  }
}

uint8_t RcvCheckSOP(){
//start with Sync-1 11000 and end with EOP 01101
  if ((RcvDataBuf[0] == 0x03) && (RcvDataBuf[RcvDataCount - 1] == 0x16)) {
    if (RcvDataBuf[1] == 0x0C) {
      //Sync-3 00110
      if ((RcvDataBuf[2] == 0x03) && (RcvDataBuf[3] == 0x0C)) {
        //SOP’’ :Sync-1 Sync-3 Sync-1 Sync-3
        //Communication to USB Type-C Plug Side B
        return 2;
      } else {
        return 0xFF;
      }
    } else if (RcvDataBuf[1] == 0x13) {
      //RST-2 11001
      if ((RcvDataBuf[2] == 0x13) && (RcvDataBuf[3] == 0x0C)) {
        //SOP’_Debug :Sync-1 RST-2 RST-2 Sync-3
        //Used for debug of USB Type-C Plug Side A
        return 3;
      }
      else if ((RcvDataBuf[2] == 0x0C) && (RcvDataBuf[3] == 0x11)) {
        //Sync-2 10001
        //SOP’’_Debug :Sync-1 RST-2 Sync-3 Sync-2
        //Used for debug of USB Type-C Plug Side B
        return 4;
      } else {
        return 0xFF;
      }
    } else {
      if (RcvDataBuf[1] == 0x03) {
        //Sync-1 Sync-1
        if ((RcvDataBuf[2] == 0x03) && (RcvDataBuf[3] == 0x11)) {
          //SOP :Sync-1 Sync-1 Sync-1 Sync-2
          //Communication to UFP
          return 0;
        } else if ((RcvDataBuf[2] == 0x0C) && (RcvDataBuf[3] == 0x0C)) {
          //SOP’ :Sync-1 Sync-1 Sync-3 Sync-3
          //Communication to USB Type-C Plug Side A
          return 1;
        } else {
          return 0xFF;
        }
      } else {
        return 0xFF;
      }
    }
  }
  return 0xFF;
}

// using comparator to receive BMC data over CC
void CMP_Interrupt() {
  //clear RcvDataBuf[73]
  __asm__(
    "    inc _XBUS_AUX                            \n"
    "    mov r7, #73                              \n"
    "    clr a                                    \n"
    "    mov dptr,#(_RcvDataBuf)                  \n"
    "loop_clr_RcvDataBuf$:                        \n"
    "    .db #0xa5                                \n"
    "    djnz r7,loop_clr_RcvDataBuf$             \n"
    "    dec _XBUS_AUX                            \n"
  );

  P1_7=1;//!!!!

  __asm__(
    "  .even                                      \n"
    "; RcvDataCount = 0;                          \n"
    "    mov	dptr,#_RcvDataCount                 \n"
    "    clr	a                                   \n"
    "    movx	@dptr,a                             \n"
    "    clr _TR0                                 \n"
    "    clr _TF0                                 \n"
    "; prepare constants                          \n"
    "    mov r7,#" STR(bCMP_IF) "                 \n"
    "    mov r6,#" STR(TL0_RECV_START_VALUE) "    \n"
    "    mov r5,#" STR(256-TL0_RECV_BIT0_UPPER_LIMIT) "\n"
    "    mov r4,#" STR(256-TL0_RECV_BIT1_UPPER_LIMIT) "\n"
    "    mov r3,#" STR(256-TL0_RECV_BIT1_LOWER_LIMIT) "\n"
    "    mov r2,0              ;RcvDataCount_local\n"
    "    mov r1,0                    ;RcvBuf_local\n"
    "    clr _pd_send_recv_flag                   \n"
    "    clr _pd_send_recv_status                 \n"
    "    nop                            ;alignment\n"
    "; wait until we get a bit 0 (3.33us)         \n"
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _TH0,r6  ;TH0 = TL0_RECV_START_VALUE;\n"
    "    clr _TF0                                 \n"
    "    setb _TR0                                \n"
    "recv_wait_for_bit_0$:                        \n"
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"
    "loop_recv_wait_for_bit_0_while$:             \n"
    //while ((ADC_CTRL & bCMP_IF) == 0)
    "    mov	a,_ADC_CTRL                         \n"
    "    jb	acc.6,loop_recv_wait_for_bit_0_flip$  \n"
    //if (TL0 >= TL0_RECV_BIT0_UPPER_LIMIT)
    "    mov	a,_TL0                              \n"
    "    add	a,r5                                \n"
    "    jnc loop_recv_wait_for_bit_0_while$      \n"
    "recv_direct_exit_1$:                         \n"
    "    clr _TR0                                 \n"
    "    clr _TF0                                 \n"
    "    ret                                      \n"
    "loop_recv_wait_for_bit_0_flip$:              \n"
    //while (TL0 < TL0_RECV_BIT1_UPPER_LIMIT)
    "    mov	a,_TL0                              \n"
    "    add	a,r4                                \n"
    "    jnc recv_wait_for_bit_0$                 \n"
    //now we are at an end of bit 0
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"
    //next bit is bit 1, we wait for it
    "loop_recv_wait_for_bit_1_in_preamble$:       \n"
    "    mov	a,_TL0                              \n"
    "    add	a,r5                                \n"
    "    jc recv_direct_exit_1$  ;just exit if > TL0_RECV_BIT0_UPPER_LIMIT \n"
    "    mov	a,_ADC_CTRL                         \n"
    "    jnb	acc.6,loop_recv_wait_for_bit_1_in_preamble$\n"
    //now we are after the middle of bit 1
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"

    "loop_recv_getting_bits$:                     \n"
    "    jb _TF0,recv_direct_exit_1$ ;just exit if TF0\n"
    "    mov	a,_ADC_CTRL                         \n"
    "    jnb	acc.6,loop_recv_getting_bits$       \n"
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"
    "; now we are just after the beginning of bit \n"

    "    mov c,_pd_send_recv_flag                 \n"
    "    mov a,r1                    ;RcvBuf_local\n"
    "    rlc a                                    \n"
    "    mov r1,a                    ;RcvBuf_local\n"
    ";check if RcvBuf_local is 0x54 or 0x56       \n"
    "    orl a,#0x2            ;0x54 will be 0x56 \n"
    "    add a,#(256-0x56)                        \n"
    "    jnz loop_recv_preamble_wait_center$      \n"
    "    setb _pd_send_recv_status                \n"
    //wait 1.56us, if it is 1 we should already passed the center
    //while (TL0 < TL0_RECV_BIT1_LOWER_LIMIT);
    "loop_recv_preamble_wait_center$:             \n"
    "    mov	a,_TL0                              \n"
    "    add	a,r3                                \n"
    "    jnc loop_recv_preamble_wait_center$      \n"
    "    mov	a,_ADC_CTRL                         \n"
    "    mov	c,acc.6                             \n"
    "    mov	_pd_send_recv_flag,c                \n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"
    "jnb _pd_send_recv_status,loop_recv_getting_bits$\n"
    //RcvDataBuf[0] = RcvDataBuf[0] & 3;
    "    mov a,r1                    ;RcvBuf_local\n"
    "    anl a,#3                                 \n"
    "    mov r1,a                    ;RcvBuf_local\n"

    




    
    //!!!!not used

    




/*
    "loop_recv_wait_for_receive_while$:           \n"
    ////another transition!
    //if ((ADC_CTRL & bCMP_IF))
    "    mov	a,_ADC_CTRL                         \n"
    "    jnb	acc.6,loop_recv_no_flip_yet$        \n"
    //wait for another transition, exit if overflow
    "loop_recv_wait_for_another_transition$:      \n"
    "    mov	a,_ADC_CTRL                         \n"
    "    jb	acc.6,loop_recv_got_another_transition$\n"
    "    jnb _TF0,loop_recv_wait_for_another_transition$\n"
    "    clr _TR0                                 \n"
    "    clr _TF0                                 \n"
    "    ret                                      \n"
    "loop_recv_got_another_transition$:           \n"
    //since we are look for preamble, we are sure we are at end of bit 1.
    //if there are 2 of 0 then the code will not work, but it is not preamble anyway.
    "    mov _TL0,r6  ;TL0 = TL0_RECV_START_VALUE;\n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"
    "    mov c,_pd_send_recv_flag                 \n"
    "    mov a,r1                    ;RcvBuf_local\n"
    "    rlc a                                    \n"
    "    mov r1,a                    ;RcvBuf_local\n"
    ";check if RcvBuf_local is 0x54 or 0x56       \n"
    "    add a,#(255-0x54)                        \n"
    "    jz end_of_recv_preamble$                 \n"
    "    mov a,r1                    ;RcvBuf_local\n"
    "    add a,#(255-0x56)                        \n"
    "    jz end_of_recv_preamble$                 \n"
    //wait 1.56us, if it is 1 we should already passed the center
    //while (TL0 < TL0_RECV_BIT1_LOWER_LIMIT);
    "loop_recv_preamble_wait_center$:             \n"
    "    mov	a,_TL0                              \n"
    "    add	a,r3                                \n"
    "    jnc loop_recv_preamble_wait_center$      \n"
    "    mov	a,_ADC_CTRL                         \n"
    "    mov	c,acc.6                             \n"
    "    mov	_pd_send_recv_flag,c                \n"
    "    mov _ADC_CTRL,r7      ;ADC_CTRL = bCMP_IF\n"



"mov _P1_7,c           \n"


    "loop_recv_no_flip_yet$:                      \n"
    //while (TL0 < TL0_RECV_BIT0_UPPER_LIMIT);
    "    mov	a,_TL0                              \n"
    "    add	a,r5                                \n"
    "    jnc loop_recv_wait_for_receive_while$    \n"

    "end_of_recv_preamble$:                       \n"
    ";because we want to be fast, we do one bit here\n"

//RcvDataBuf[0] = RcvDataBuf[0] & 3;

"clr _P1_7           \n"
"setb _P1_7           \n"



    "    ljmp not_return_from_recv$     \n"
    "return_from_recv$:                           \n"
    "   ret                           \n"
    "not_return_from_recv$:                           \n"*/
  );

P1_7=0; //!!!!!

        while ( ((ADC_CTRL & bCMP_IF)==0) && (TL0 < TL0_RECV_BIT0_UPPER_LIMIT) );
        B = ((ADC_CTRL & bCMP_IF) != 0);
        if (B) {
          ADC_CTRL = bCMP_IF;
        }

return;



  __data uint8_t preambleFlag = 1;
  __data uint8_t bitmask = 3;
  __data uint8_t tempA = 0;

  TL0 = TL0_RECV_START_VALUE;
  TH0 = TL0_RECV_START_VALUE;
  TF0 = 0;
  TR0 = 1;
  RcvDataCount = 0;
  //for BMC@300K,each 0 is 3.33us and 1 is 1.66us*2
  do {
    TL0 = TL0_RECV_START_VALUE;
    ADC_CTRL = bCMP_IF;
    while ((ADC_CTRL & bCMP_IF) == 0) {
      //121@32M = 3.78us, longer than a regular bit
      if (TL0 >= TL0_RECV_BIT0_UPPER_LIMIT) {
        return;
      }
    }
    //87@32M = 2.72us, try until we find a bit 0
  } while (TL0 < TL0_RECV_BIT1_UPPER_LIMIT);

  //now we are at an end of bit 0
  TL0 = TL0_RECV_START_VALUE;
  ADC_CTRL = bCMP_IF;
  do {
    //another transition!
    if ((ADC_CTRL & bCMP_IF)) {
      TL0 = TL0_RECV_START_VALUE;
      ADC_CTRL = bCMP_IF;
      do {
        //wait for another transition
        while ((ADC_CTRL & bCMP_IF) == 0) {
          //I guess this is faster than compare?
          if (TF0) {
            TR0 = 0;
            TF0 = 0;
            return;
          }
        }
        //since we are look for preamble, we are sure we are at end of bit 1.
        //if there are 2 of 0 then the code will not work, but it is not preamble anyway.
        TL0 = TL0_RECV_START_VALUE;
        ADC_CTRL = bCMP_IF;
        RcvDataBuf[0] = (RcvDataBuf[0] << 1) + tempA;
        //last 2 bit are 00 or 10, that is the end of preamble
        //All SOP start with Sync-1 K-code 11000, at LSB first, that is 00
        //Cable Reset and Hard Reset start with RST-1 K-code 00111, can ignore.
        //not sure why we need to check the case of 10
        if ((RcvDataBuf[0] == 0x54) || (RcvDataBuf[0] == 0x56)) {
          RcvDataBuf[0] = RcvDataBuf[0] & 3;
          preambleFlag = 0;
        }
        //wait 1.56us, if it is 1 we should already passed the center
        while (TL0 < TL0_RECV_BIT1_LOWER_LIMIT);
        tempA = ((ADC_CTRL & bCMP_IF) != 0);
        if (tempA) {
          ADC_CTRL = bCMP_IF;
        }
      } while (preambleFlag != 0);
      //now we are 2 bits beyond preamble
      do {
        while ((ADC_CTRL & bCMP_IF)) {
          TL0 = TL0_RECV_START_VALUE;
          ADC_CTRL = 0x40;
          RcvDataBuf[RcvDataCount] = (RcvDataBuf[RcvDataCount] << 1) + tempA;
          //each byte contains 5bits.
          bitmask--;
          if (bitmask == 0) {
            bitmask = 5;
            RcvDataCount++;
          }
          //wait 1.56us, if it is 1 we should already passed the center
          while (TL0 < TL0_RECV_BIT1_LOWER_LIMIT);
          if ((ADC_CTRL & bCMP_IF) == 0) {
            tempA = false;
          }
          else {
            ADC_CTRL = bCMP_IF;
            tempA = true;
          }
        }
      } while (TF0 == 0);
      TF0 = 0;
      TR0 = 0;
      return;
    }
    //as long as we are shorter than a regular bit
  } while (TL0 < TL0_RECV_BIT0_UPPER_LIMIT);
}

void SEND_INTERRUPT(){
  if ( (CCSel == 1) || (CCSel == 2) ){
    __data uint8_t toggleMask = 1<<(3+CCSel);
    __bit sendBit = 1;
    TR0 = 0;
    TF0 = 0;
    TH0 = TL0_SEND_START_VALUE;
    TL0 = TL0_SEND_START_VALUE;
    __data uint8_t sendCounter = 63;
    //sending 01010101.... end of 1
    //send 63 bits of 10101....01
    
    P1 &= ~toggleMask;
    P1_MOD_OC &= ~toggleMask;
    P1_DIR_PU |= toggleMask;

    TR0 = 1;
    do{
      //wait till overflow */
      while(TF0==0); 
      P1^=toggleMask;
      TF0 = 0;
      //1.435us (196-150)@32M
      //toggle in middle if we have 1
      while(TL0<TL0_SEND_BIT1_TOGGLE); 
      if (sendBit){
        P1^=toggleMask;
      }
      sendBit = !sendBit;
      sendCounter--; 
    }while(sendCounter != 0);
    //now we send data
    __data uint8_t dataSendMask = 1;
    do{
      //wait till overflow */
      while(TF0==0); 
      P1^=toggleMask;
      TF0 = 0;
      // use a mask to get a bit
      sendBit = (SndDataBuf[sendCounter]&dataSendMask)!=0;
      dataSendMask<<=1;
      while(TL0<TL0_SEND_BIT1_TOGGLE); 
      if (sendBit){
        P1^=toggleMask;
      }
      // check if we already done 5 bits
      if (dataSendMask == (1<<5)){
        dataSendMask = 1;
        sendCounter++;
      }
    }while( SndDataCount != sendCounter);
    while(TF0==0); 
    TF0 = 0;
    P1^=toggleMask;
    delayMicroseconds(2); //need fine tune delay  !!!!!
    P1|=toggleMask;
    delayMicroseconds(1);

    P1_MOD_OC &= ~toggleMask;
    P1_DIR_PU &= ~toggleMask;
  }
  TR0 = 0;
  TF0 = 0;
  SndDone = 1;
}

uint8_t SendHandle(){
  SndDataCount = Union_Header->HeaderStruct.NDO * 4 + 2;
  *((uint32_t *)(&SndDataBuf[SndDataCount])) = CalculateCRC(((uint32_t)SndDataBuf)|( ((uint32_t)SndDataCount) <<16) );

  // 4b5b encoding
  __data uint8_t i = SndDataCount+4;
  do{
    i--;
    SndDataBuf[4+i*2+0] = Cvt4B5B[SndDataBuf[i] & 0x0f];
    SndDataBuf[4+i*2+1] = Cvt4B5B[SndDataBuf[i] >> 4];
  }while(i!=0);
  // (data+crc)*2 + SOP + EOP 
  SndDataCount = (SndDataCount+4)*2+4+1;
  // EOP 01101
  SndDataBuf[SndDataCount-1] = 0x0d;
  SndDataBuf[0] = 0x18;
  switch (RecvSop){
    case 0:
      //got SOP :Sync-1 Sync-1 Sync-1 Sync-2
      SndDataBuf[1] = 0x18;
      SndDataBuf[2] = 0x18;
      SndDataBuf[3] = 0x11;
      break;
    case 1:
      //got SOP’ :Sync-1 Sync-1 Sync-3 Sync-3
      SndDataBuf[1] = 0x18;
      SndDataBuf[2] = 0x06;
      SndDataBuf[3] = 0x06;
      break;
    case 2:
      //got SOP’’ :Sync-1 Sync-3 Sync-1 Sync-3
      SndDataBuf[1] = 0x06;
      SndDataBuf[2] = 0x18;
      SndDataBuf[3] = 0x06;
      break;
    case 3:
      //got SOP’_Debug :Sync-1 RST-2 RST-2 Sync-3
      SndDataBuf[1] = 0x19;
      SndDataBuf[2] = 0x19;
      SndDataBuf[3] = 0x06;
      break;
    case 4:
      //got SOP’’_Debug :Sync-1 RST-2 Sync-3 Sync-2
      SndDataBuf[1] = 0x19;
      SndDataBuf[2] = 0x06;
      SndDataBuf[3] = 0x11;
      break;
    default:
      break;
  }

  __data uint8_t RetryCount = 3;
  do{
    E_DIS = 1;
    SEND_INTERRUPT();
    E_DIS = 0;

    //never happen as SEND_INTERRUPT always success?
    if (SndDone == 0) {  
      return 1;
    }
    SndDone = 0;
    if (SendingGoodCRCFlag != 0) {
      SndDone = 0;
      SendingGoodCRCFlag = 0;
      return 0;
    }

    if (SndMsgID < 7) {
      SndMsgID = SndMsgID + 1;
    } else {
      SndMsgID = 0;
    }

    ADC_CHAN = CCSel + 3 | 0x30;
    __xdata uint16_t TimeOutCount;
    TimeOutCount = 0;
    ADC_CTRL = bCMP_IF;

    do{
      if ((ADC_CTRL & bCMP_IF) != 0){
        E_DIS = 1;
        CMP_Interrupt();
        E_DIS = 0;

        if (RcvDataCount == 0) {
          return 1;
        }

        __data uint8_t newRecvSop = RcvCheckSOP();
        // wch code check if last byte of buffer is 0x16, not sure why
        if (RecvSop != newRecvSop) {
          return 1;
        }

        RcvBufDecode5B4B();

        Union_Header = (__xdata _Union_Header * __data)RcvDataBuf;
        if (Union_Header->HeaderStruct.MsgType == 1) {
          SendingGoodCRCFlag = 0;
          return 0;
        }
        return 1;

      }

      delayMicroseconds(1);
      TimeOutCount++;
    }while(TimeOutCount < TIMEOUTCOUNT_LIMIT);

    delayMicroseconds(2000);
    RetryCount--;
  }while (RetryCount!=0);

  return 1;
}


uint8_t ReceiveHandle() {
  __xdata uint16_t TimeOutCount;
  // setting comparator channel to receive CC data 
  ADC_CHAN = CCSel + 3 | 0x30;
  TimeOutCount = 0;
  ADC_CTRL = bCMP_IF;
  while ((ADC_CTRL & bCMP_IF) == 0) {
    TimeOutCount++;
    //need more investigation what time it means. ( (34+3+(2or3))*700 clks? 0.85ms? 256bits of BMC?) this SDCC code need more tuning
    if (TimeOutCount >= TIMEOUTCOUNT_LIMIT) {  
      return NODATA;
    }
  }
  E_DIS = 1;
  CMP_Interrupt();
  E_DIS = 0;
P1_7=1; ///!!!!
P1_7=0;
P1_7=1;
P1_7=0;

  if (RcvDataCount == 0) {
    return NODATA;
  }

  RecvSop = RcvCheckSOP();
  if (RecvSop == 0xFF) {
    return ILLEGAL;
  }
  
  RcvBufDecode5B4B();

  //passing ptr in DPTR and length in B
  __data uint8_t dataBeforeCRC = (2+((_Msg_Header_Struct *)(RcvDataBuf))->NDO*4);
  __data uint32_t crc = CalculateCRC(((uint32_t)RcvDataBuf)|( ((uint32_t)dataBeforeCRC) <<16) );
  if (crc != *((uint32_t *)(&RcvDataBuf[dataBeforeCRC]))) {
    //CRC error
    return ILLEGAL;
  }
  //maybe redo data count?

  Union_Header = (__xdata _Union_Header * __data)RcvDataBuf;
  RcvMsgID = Union_Header->HeaderStruct.MsgID;
  ResetSndHeader();

  if (((_Msg_Header_Struct *)(RcvDataBuf))->PortPwrRole){
      //reverse power role between Sink & Source
      //pointer Union_Header has been modified by ResetSndHeader()
      Union_Header->HeaderStruct.PortPwrRole = 0;
  }else{
      Union_Header->HeaderStruct.PortPwrRole = 1;
  }

  if (((_Msg_Header_Struct *)(RcvDataBuf))->PortDataRole){
      //reverse PortDataRole between UFP & DFP
      //pointer Union_Header has been modified by ResetSndHeader()
      Union_Header->HeaderStruct.PortDataRole = 0;
  }else{
      Union_Header->HeaderStruct.PortDataRole = 1;
  }

  Union_Header->HeaderStruct.MsgID = RcvMsgID;
  Union_Header->HeaderStruct.MsgType = GoodCRC;

  SendingGoodCRCFlag = 1;

  SendHandle();

  Union_Header = (__xdata _Union_Header * __data)RcvDataBuf;

  return REVSUCCESS;
}
