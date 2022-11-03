void captureInline5mhz() {

  //  basic trigger, wait until all trigger conditions are met on port.
  //  this needs further testing, but basic tests work as expected.

  //  disable interrupts during capture to maintain precision.
  //  we cannot afford any timing interference so we absolutely
  //  cannot have any interrupts firing.

  EA = 0;

  //prepare R0 DPTR
  __asm__("  mov r0,#_swapByte        \n"
          "  inc _XBUS_AUX            \n"  //select DPTR1
          "  mov dptr,#(_logicdata-0) \n"
          "  dec _XBUS_AUX            \n"  //select DPTR0 , set it back
         );

  __asm__("  sjmp notRet5M$       \n" //only return when UEP2_CTRL changes

          "abortSample5M$:        \n"
          "  setb  _EA            \n"
          "  ret                  \n"

          "notRet5M$:"
          "  mov a,_trigger       \n" //if trigger is set
          "  jz  NoTrigger5M$     \n"
          "  mov r5,_UEP2_CTRL    \n"  //backup UEP2_CTRL

          "checkTrigger5M$:"

          "  mov a,_UEP2_CTRL     \n" //abort if UEP2_CTRL changes
          "  xrl a,r5             \n"
          "  jnz abortSample5M$   \n"

          "  mov a,_P1            \n"  //(trigger_values ^ P1) & trigger. If result is 0, got triggered.
          "  xrl a,_trigger_values\n"
          "  anl a,_trigger       \n"
          "  jnz checkTrigger5M$  \n"

          "NoTrigger5M$:          \n"
         );

  //P1 is at address 0x90, but it can not be indirectly accessed, because it is on "top" of the ram

  //a5-> MOVX @DPTR1,A & INC DPTR1

  //===768 pairs

  __asm__("  mov r6,#153      \n" //opcode 7E 99

          ".even \n loop10Samples$:    \n" //new linking file ensured each file start at even address, so even in one file is even in final, faster for jnz 

          "  mov a,_P1 \n  swap a \n mov @r0,a \n " //opcode E5 90 C4 F6, 4CLK
          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n" //4CLK

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

          "  nop      \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

          "  nop      \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

          "  dec r6      \n"

          "  mov a,_P1 \n  swap a \n mov @r0,a \n "
          "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

          "  mov a,r6      \n"  //1+2+2 = 5clk for loop
          "  jnz loop10Samples$\n");


  //765 pair finished, 3 more needed.
  __asm__(
    "  mov a,_P1 \n  swap a \n mov @r0,a \n "
    "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

    "  nop      \n"

    "  mov a,_P1 \n  swap a \n mov @r0,a \n "
    "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n"

    "  nop      \n"

    "  mov a,_P1 \n  swap a \n mov @r0,a \n "
    "  mov a,_P1 \n  xchd A,@r0 \n  .db #0xa5 \n");

  //===768 pairs end


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
