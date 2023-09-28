/*
 created by Deqing Sun for use with CH55xduino
 */

#define ARDUINO_MAIN

// clang-format off
#include "wiring_private.h"

uint16_t digitalReadDirectLutAddrAndCarryReturn(uint8_t pin);

#if F_CPU == 24000000
  #define F_CPU_MHZ 24
#elif F_CPU == 16000000
  #define F_CPU_MHZ 16
#elif F_CPU == 12000000
  #define F_CPU_MHZ 12
#elif F_CPU == 32000000
  #define F_CPU_MHZ 32
#elif F_CPU == 56000000
  #define F_CPU_MHZ 56
#endif

#define STR_INDIR(x) #x
#define STR(x) STR_INDIR(x)

uint32_t pulseIn(uint8_t pin, __xdata uint8_t state, __xdata uint16_t timeout) {
  pin;
  state;
  timeout;

  // clang-format off
  __asm__(
    //".even                                        \n"
    ";get address of the pin read instruction     \n"
    ";dpl is already ready                        \n"
    "    lcall _digitalReadDirectLutAddrAndCarryReturn \n"
    "; backup dptr                                \n"

    ";put loop limit (inc to overflow)            \n"
    ";roughly accurate.                           \n"
    ";value = 65536-(ms*F_CPU*(1E-6)*34)          \n"
    ";that is 16bit x 16 bit = 32bit              \n"
    ";https://developer.arm.com/documentation/ka002453/latest/ \n"
    "    inc _XBUS_AUX                            \n"  //select DPTR1
    "    mov dptr,#_pulseIn_PARM_3                \n"
    "    movx a,@dptr                             \n" //low byte of timeout
    "    mov b,#(("STR(F_CPU_MHZ)"*34)&0XFF)      \n"
    "    mul ab                                   \n"
    "    mov r1,a                                 \n" //lowest byte of result, we use r1, r2 ,r3 ,r4 for now
    "    mov r2,b                                 \n" //store 2nd byte temporarily
    "    movx a,@dptr                             \n" //low byte of timeout
    "    mov b,#(("STR(F_CPU_MHZ)"*34)>>8)        \n"
    "    mul ab                                   \n"
    "    add a,r2                                 \n" 
    "    mov r2,a                                 \n" //store 2nd byte temporarily
    "    mov a, #0                                \n" 
    "    addc a,b                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte temporarily
    "    inc dptr                                 \n" 
    "    movx a,@dptr                             \n" //high byte of timeout
    "    mov b,#(("STR(F_CPU_MHZ)"*34)&0XFF)      \n"
    "    mul ab                                   \n"
    "    add a,r2                                 \n"
    "    mov r2,a                                 \n" //store 2nd byte
    "    mov a,r3                                 \n" //restore 3rd byte
    "    addc a,b                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte temporarily
    "    mov a,#0                                 \n"
    "    addc a,#0                                \n"
    "    mov r4,a                                 \n" //store 4th byte temporarily, there might be carry from 3rd byte
    "    movx a,@dptr                             \n" //high byte of timeout
    "    mov b,#(("STR(F_CPU_MHZ)"*34)>>8)        \n"
    "    mul ab                                   \n"
    "    add a,r3                                 \n"
    "    mov r3,a                                 \n" //store 3rd byte
    "    mov a,r4                                 \n" //restore 4th byte
    "    addc a,b                                 \n"
    "    mov r4,a                                 \n" //store 4th byte
    "  ;65536-result                              \n"
    "    clr a                                    \n"
    "    subb a,r1                                \n"
    "    mov r0,a                                 \n" 
    "    clr a                                    \n"
    "    subb a,r2                                \n"
    "    mov r1,a                                 \n"
    "    clr a                                    \n"
    "    subb a,r3                                \n"
    "    mov r2,a                                 \n"
    "    clr a                                    \n"
    "    subb a,r4                                \n"
    "    mov r3,a                                 \n"
    "    dec _XBUS_AUX                            \n"  //select DPTR0 , set it back 

    ";branching if we meausure high pulse of low  \n"
    "    inc _XBUS_AUX                            \n"  //select DPTR1
    "    mov	dptr,#_pulseIn_PARM_2               \n"  //do not modify the DPTR0 for read pin
    "    movx	a,@dptr                             \n"
    "    dec _XBUS_AUX                            \n"  //select DPTR0 , set it back 
    "    jz lowPulseDetection$                    \n"

    ";highPulse, Wait for low, previous pulse end\n"
    "highPulseWaitPrevFinish$:                    \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highPulseWaitPrevFinish_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "highPulseWaitPrevFinish_overflow_countOver$: \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc highPulseWaitPrevFinish$              \n"
    ";                                            \n"
    ";highPulse, Wait for rise                    \n"
    "highPulseWaitRise$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highPulseWaitRise_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highPulseWaitRise_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highPulseWaitRise_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highPulseWaitRise_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "highPulseWaitRise_overflow_countOver$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc highPulseWaitRise$                   \n"
    ";highPulse, got rising edge, record micros   \n"
    "    inc _XBUS_AUX                            \n"  //select DPTR1
    "    push ar0                                 \n"
    "    push ar1                                 \n"
    "    push ar2                                 \n"
    "    push ar3                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar3                                  \n"
    "    pop ar2                                  \n"
    "    pop ar1                                  \n"
    "    pop ar0                                  \n"
    "    mov	r4,dpl                              \n"  //do not modify the DPTR0 for read pin
    "    mov	r5,dph                              \n"
    "    mov  r6,b                                \n"
    "    mov  r7,a                                \n"
    "    dec _XBUS_AUX                            \n"  //select DPTR0 , set it back 
    ";                                            \n"
    ";highPulse, Wait for fall                    \n"
    "highPulseWaitFall$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,highPulseWaitFall_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,highPulseWaitFall_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,highPulseWaitFall_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,highPulseWaitFall_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "highPulseWaitFall_overflow_countOver$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc highPulseWaitFall$                    \n"
    ";highPulse, got falling edge, record micros  \n"
    ";dptr no longer needed, r0~3 no longer needed\n"
    ";call micros again to get the difference     \n"
    "    ljmp calculateMicrosDiff$                \n"

    "lowPulseDetection$:                          \n"

    ";lowPulse, Wait for high, previous pulse end\n"
    "lowPulseWaitPrevFinish$:                    \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowPulseWaitPrevFinish_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowPulseWaitPrevFinish_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "lowPulseWaitPrevFinish_overflow_countOver$: \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc lowPulseWaitPrevFinish$              \n"
    ";                                            \n"
    ";lowPulse, Wait for fall                    \n"
    "lowPulseWaitFall$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowPulseWaitFall_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowPulseWaitFall_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowPulseWaitFall_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowPulseWaitFall_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "lowPulseWaitFall_overflow_countOver$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jnc lowPulseWaitFall$                   \n"
    ";lowPulse, got falling edge, record micros   \n"
    "    inc _XBUS_AUX                            \n"  //select DPTR1
    "    push ar0                                 \n"
    "    push ar1                                 \n"
    "    push ar2                                 \n"
    "    push ar3                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar3                                  \n"
    "    pop ar2                                  \n"
    "    pop ar1                                  \n"
    "    pop ar0                                  \n"
    "    mov	r4,dpl                              \n"  //do not modify the DPTR0 for read pin
    "    mov	r5,dph                              \n"
    "    mov  r6,b                                \n"
    "    mov  r7,a                                \n"
    "    dec _XBUS_AUX                            \n"  //select DPTR0 , set it back 
    ";                                            \n"
    ";lowPulse, Wait for rise                    \n"
    "lowPulseWaitRise$:                          \n"
    "    inc r0                                   \n"
    "    cjne r0,#0,lowPulseWaitRise_overflow_countOver$\n"
    "    inc r1                                   \n"
    "    cjne r1,#0,lowPulseWaitRise_overflow_countOver$\n"
    "    inc r2                                   \n"
    "    cjne r2,#0,lowPulseWaitRise_overflow_countOver$\n"
    "    inc r3                                   \n"
    "    cjne r3,#0,lowPulseWaitRise_overflow_countOver$\n"
    ";32bit overflow if get here                  \n"
    "    ljmp failReturn0$                        \n"
    "lowPulseWaitRise_overflow_countOver$:       \n"
    "    lcall __sdcc_call_dptr                   \n"
    "    jc lowPulseWaitRise$                    \n"
    ";lowPulse, got rising edge, record micros  \n"
    ";dptr no longer needed, r0~3 no longer needed\n"
    ";call micros again to get the difference     \n"
    "    ljmp calculateMicrosDiff$                \n"

    "calculateMicrosDiff$:                        \n"
    "    push ar4                                 \n"
    "    push ar5                                 \n"
    "    push ar6                                 \n"
    "    push ar7                                 \n"
    "    lcall	_micros                           \n"
    "    pop ar7                                  \n"
    "    pop ar6                                  \n"
    "    pop ar5                                  \n"
    "    pop ar4                                  \n"
    "    mov	r0,a                                \n"
    "    clr c                                    \n"
    "    mov	a,dpl                               \n"
    "    subb	a,r4                                \n"
    "    mov	dpl,a                               \n"
    "    mov	a,dph                               \n"
    "    subb	a,r5                                \n"
    "    mov	dph,a                               \n"
    "    mov	a,b                                 \n"
    "    subb	a,r6                                \n"
    "    mov	b,a                                 \n"
    "    mov	a,r0                                \n"
    "    subb	a,r7                                \n"
    "    ret                                      \n"
 
    "failReturn0$:                                \n"
    "    clr a                                    \n"
    "    mov dpl,a                                \n"
    "    mov dph,a                                \n"
    "    mov b,a                                  \n"
    "    ret                                      \n"
  );
  // clang-format on  
}
