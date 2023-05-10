void captureInlineWithT2() {

  //  basic trigger, wait until all trigger conditions are met on port.
  //  this needs further testing, but basic tests work as expected.

  //  disable interrupts during capture to maintain precision.
  //  we cannot afford any timing interference so we absolutely
  //  cannot have any interrupts firing.

  //Timer0 used for millis, Timer1 used for serial0
  //we can only use Timer2
  TR2 = 0;

  RCLK = 0;
  TCLK = 0;
  C_T2 = 0;

  //bTMR_CLK may be set by uart0, we keep it as is.
  T2MOD |= bTMR_CLK | bT2_CLK; //use Fsys for T2

  CP_RL2 = 0;

  if ((divider) > (273064L)) {
    RCAP2L = 0;
    RCAP2H = 0;
  } else {
    uint16_t reloadValueT2 = (uint16_t)(65536L - ((uint16_t)((divider + 1) * 6L / 25)));
    RCAP2L = reloadValueT2 & 0xFF;
    RCAP2H = (reloadValueT2 >> 8) & 0xFF;
  }
  TL2 = RCAP2L;
  TH2 = RCAP2H;

  TF2 = 0;

  EA = 0;

  //prepare R0 DPTR
  __asm__("  mov r0,#_swapByte        \n"
          "  inc _XBUS_AUX            \n"  //select DPTR1
          "  mov dptr,#(_logicdata-0) \n"
          "  dec _XBUS_AUX            \n"  //select DPTR0 , set it back
         );

  __asm__("  sjmp notRetT2$       \n" //only return when UEP2_CTRL changes

          "abortSampleT2$:        \n"
          "  setb  _EA            \n"
          "  ret                  \n"

          "notRetT2$:"
          "  mov a,_trigger       \n" //if trigger is set
          "  jz  NoTriggerT2$     \n"
          "  mov r5,_UEP2_CTRL    \n"  //backup UEP2_CTRL

          "checkTriggerT2$:"

          "  mov a,_UEP2_CTRL     \n" //abort if UEP2_CTRL changes
          "  xrl a,r5             \n"
          "  jnz abortSampleT2$   \n"

          "  mov a,_P1            \n"  //(trigger_values ^ P1) & trigger. If result is 0, got triggered.
          "  xrl a,_trigger_values\n"
          "  anl a,_trigger       \n"
          "  jnz checkTriggerT2$  \n"

          "NoTriggerT2$:          \n"
         );

  //P1 is at address 0x90, but it can not be indirectly accessed, because it is on "top" of the ram

  //a5-> MOVX @DPTR1,A & INC DPTR1

  //===740 pairs

  TR2 = 1;
  uint8_t i = 185;

#define INLINE_CAPTURE1   __asm__("  mov a,_P1 \n  swap a \n mov @r0,a \n " );
#define INLINE_CAPTURE2   __asm__("  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n" );


  __asm__("  mov r6,#185      \n"

          ".even \n loop8Samples$:    \n" //new linking file ensured each file start at even address, so even in one file is even in final, faster for jnz 

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "waitTF2_1$: \n  jnb _TF2,waitTF2_1$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"
          "waitTF2_2$: \n  jnb _TF2,waitTF2_2$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "waitTF2_3$: \n  jnb _TF2,waitTF2_3$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"
          "waitTF2_4$: \n  jnb _TF2,waitTF2_4$ \n  clr _TF2 \n"

          "  dec r6      \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "waitTF2_5$: \n  jnb _TF2,waitTF2_5$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"
          "waitTF2_6$: \n  jnb _TF2,waitTF2_6$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "waitTF2_7$: \n  jnb _TF2,waitTF2_7$ \n  clr _TF2 \n"

          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"
          "waitTF2_8$: \n  jnb _TF2,waitTF2_8$ \n  clr _TF2 \n"

          "  mov a,r6      \n"  //1+2+2 = 5clk for loop
          "  jnz loop8Samples$\n");


  //===740 pairs end


  // re-enable interrupts now that we're done sampling.
  EA = 1;

  //   dump the samples back to the SUMP client.  nothing special
  //   is done for any triggers, this is effectively the 0/100 buffer split.

  //pulseview use reversed order

  for (unsigned int i = 0 ; i < readCount; i++ ) {
    unsigned int rev_i = readCount - i - 1;
    if ((rev_i & 1) == 0) {
      USBSerial_write(logicdata[rev_i / 2] & 0x0F);
    } else {
      USBSerial_write((logicdata[rev_i / 2]) >> 4);
    }
  }

}
