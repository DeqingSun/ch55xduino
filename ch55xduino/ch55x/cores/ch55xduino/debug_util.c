// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
// clang-format on

// in SDCC, caller will save the registers (R0-R7)
// DPL, DPH, B and ACC are for parameter/return value passing
// At this moment, sendCharDebug is not included in any header
// use pragma callee_saves when you declare sendCharDebug
#pragma callee_saves sendCharDebug
void sendCharDebug(__data char c) // 8Mbps under 24M clk
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
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  rrc a             \n"
      "  mov _P1_4,c       \n"
      "  setb c            \n"
      "  mov _P1_4,c       \n");
  // if (interruptOn) EA = 1;

  __asm__("  mov a,b           \n"
          "  jz skipSetEADebug$\n"
          "  setb _EA          \n"
          "skipSetEADebug$:    \n");
}

/*
void    mDelayuS( uint16_t n )
{
#ifdef    F_CPU
#if        F_CPU <= 6000000
    n >>= 2;
#endif
#if        F_CPU <= 3000000
    n >>= 2;
#endif
#if        F_CPU <= 750000
    n >>= 4;
#endif
#endif
    while ( n ) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
        ++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef    F_CPU
#if        F_CPU >= 14000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 16000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 18000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 20000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 22000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 24000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 26000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 28000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 30000000
        ++ SAFE_MOD;
#endif
#if        F_CPU >= 32000000
        ++ SAFE_MOD;
#endif
#endif
        -- n;
    }
}

void    mDelaymS( uint16_t n ){
    while ( n ) {
#ifdef    DELAY_MS_HW
        while ( ( TKEY_CTRL & bTKC_IF ) == 0 );
        while ( TKEY_CTRL & bTKC_IF );
#else
        mDelayuS( 1000 );
#endif
        -- n;
    }
}*/
