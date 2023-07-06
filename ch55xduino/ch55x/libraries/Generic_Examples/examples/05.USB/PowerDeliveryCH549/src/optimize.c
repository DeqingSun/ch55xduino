#include "common.h"

void RcvBufDecode5B4B(){
  //now we deal with header
  //the SOP is no longer needed, reuse the space for decoded data

  //a5-> MOVX @DPTR1,A & INC DPTR1

  __asm__(
    " ; write DPTR1 as write ptr, use with 0xa5   \n"
    "    inc _XBUS_AUX                            \n"
    "    mov dptr,#(_RcvDataBuf+0)                \n"
    "    dec _XBUS_AUX                            \n"

    "    mov dptr,#(_RcvDataBuf+4)                \n"
    "    mov r3,#2                                \n"

    "RcvBufDecode5B4B_loop_2B$:                   \n"

    " ; write DPTR as read ptr, use with r7r6 swap\n"
    " ; get 2 bytes from RcvDataBuf               \n"
    "    movx a,@dptr                             \n"
    "    inc dptr                                 \n"
    "    mov r4,a                  ;r4 low 4bit 5B\n"
    "    movx a,@dptr                             \n"
    "    inc dptr                  ;a high 4bit 5B\n"

    " ; get 2 bytes from RcvDataBuf               \n"
    "    mov r7,dpl                  ;r7 store dpl\n"
    "    mov r6,dph                  ;r6 store dph\n"
    "    mov dptr,#(_Cvt5B4B)                     \n"
    "    movc a,@a+dptr                           \n"
    "    swap a                                   \n"
    "    mov r5,a                                 \n"
    "    mov dptr,#(_Cvt5B4B)                     \n"
    "    mov a,r4                                 \n"
    "    movc a,@a+dptr                           \n"
    "    orl a,r5                                 \n"
    "    mov dpl,r7                               \n"
    "    mov dph,r6                               \n"

    " ; write result back to RcvDataBuf           \n"
    "    .db #0xa5                                \n"
    "    djnz r3,RcvBufDecode5B4B_loop_2B$        \n"

    //leftOverData = (((_Msg_Header_Struct *)(RcvDataBuf))->NDO*4+4)*2
    //NDO is bit4~6 of RcvDataBuf[1]
    //NDO is the number of data objects, each data object is 4 bytes, and 4 byte CRC. each byte is 2 5b

    "    mov dptr,#(_RcvDataBuf+1)                \n"
    "    movx a,@dptr                             \n"
    "    swap a                                   \n"
    "    anl a,#0x07                              \n"
    "    add a,acc                                \n"
    "    add a,acc                                \n"
    "    add a,#0x04                              \n"
    "    add a,acc                                \n"
    "    mov r3,a                                 \n"
    "    mov dpl,r7                               \n"
    "    mov dph,r6                               \n"

    "RcvBufDecode5B4B_loop_REST$:                 \n"
    " ; write DPTR as read ptr, use with r7r6 swap\n"
    " ; get 2 bytes from RcvDataBuf               \n"
    "    movx a,@dptr                             \n"
    "    inc dptr                                 \n"
    "    mov r4,a                  ;r4 low 4bit 5B\n"
    "    movx a,@dptr                             \n"
    "    inc dptr                  ;a high 4bit 5B\n"

    " ; get 2 bytes from RcvDataBuf               \n"
    "    mov r7,dpl                  ;r7 store dpl\n"
    "    mov r6,dph                  ;r6 store dph\n"
    "    mov dptr,#(_Cvt5B4B)                     \n"
    "    movc a,@a+dptr                           \n"
    "    swap a                                   \n"
    "    mov r5,a                                 \n"
    "    mov dptr,#(_Cvt5B4B)                     \n"
    "    mov a,r4                                 \n"
    "    movc a,@a+dptr                           \n"
    "    orl a,r5                                 \n"
    "    mov dpl,r7                               \n"
    "    mov dph,r6                               \n"

    " ; write result back to RcvDataBuf           \n"
    "    .db #0xa5                                \n"
    "    djnz r3,RcvBufDecode5B4B_loop_REST$      \n"

  );
}

void SndBufEncode4B5B(){
  __asm__(
    "    mov dptr,#_SndDataCount                  \n"
    "    movx a,@dptr                             \n"
    "    mov r7,a                                 \n"

    " ; r5:r6 is the write ptr SndDataBuf[4+2*(SndDataCount-1)] \n"
    "    dec a                                    \n"
    "    mov r5,a                ;read ptr offset \n"
    "    add a,acc                                \n"
    "    add a,#04                                \n"
    "    mov r6,a                ;write ptr offset\n"
    "    mov r4,#0                                \n"

    "SndBufEncode4B5B_loop$:                      \n"
    "    mov a,#(_SndDataBuf)                     \n"
    "    add a,r5                                 \n"
    "    mov dpl,a                                \n"
    "    mov a,#(_SndDataBuf >> 8)                \n"
    "    addc a,r4                     ;r4 keep 0 \n"
    "    mov dph,a                                \n"
    "    movx a,@dptr                             \n"
    "    mov r3,a                  ;SndDataBuf[i] \n"

    "    anl a,#0xF0                              \n"
    "    swap a                                   \n"
    "    mov dptr,#(_Cvt4B5B)                     \n"
    "    movc a,@a+dptr                           \n"
    "    mov r2,a   ; Cvt4B5B[SndDataBuf[i] >> 4] \n"
    "    mov a,r3                                 \n"
    "    anl a,#0x0F                              \n"
    "    mov dptr,#(_Cvt4B5B)                     \n"
    "    movc a,@a+dptr                           \n"
    "    mov r1,a  ;Cvt4B5B[SndDataBuf[i] & 0x0f] \n"
    "    dec r5                                   \n"

    "    mov a,#(_SndDataBuf)                     \n"
    "    add a,r6                                 \n"
    "    mov dpl,a                                \n"
    "    mov a,#(_SndDataBuf >> 8)                \n"
    "    addc a,r4                     ;r4 keep 0 \n"
    "    mov dph,a                                \n"
    "    mov a,r1                                 \n"
    "    movx @dptr,a                             \n"
    "    inc dptr                                 \n"
    "    mov a,r2                                 \n"
    "    movx @dptr,a                             \n"
    "    dec r6                                   \n"
    "    dec r6                                   \n"

    "    djnz r7,SndBufEncode4B5B_loop$           \n"
  );
}

/*
void SndBufEncode4B5B(){
  __data uint8_t i = SndDataCount;
  do{
    i--;
    SndDataBuf[4+i*2+0] = Cvt4B5B[SndDataBuf[i] & 0x0f];
    SndDataBuf[4+i*2+1] = Cvt4B5B[SndDataBuf[i] >> 4];
  }while(i!=0);
}
*/