/*
 created by Deqing Sun for use with CH55xduino
 */

#include "wiring_private.h"

void nothing(void) {}

__xdata voidFuncPtr intFunc[EXTERNAL_NUM_INTERRUPTS] = {
#if EXTERNAL_NUM_INTERRUPTS > 1
    nothing,
#endif
#if EXTERNAL_NUM_INTERRUPTS > 0
    nothing,
#endif
};

void attachInterrupt(__data uint8_t interruptNum,
                     __xdata void (*userFunc)(void), __xdata uint8_t mode) {
  if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    intFunc[interruptNum] = userFunc;

    // Configure the interrupt mode (trigger on low input, any change, rising
    // edge, or falling edge).  The mode constants were chosen to correspond
    // to the configuration bits in the hardware register, so we simply shift
    // the mode into place.

    // Enable the interrupt.

    switch (interruptNum) {
    case 0:
      IT0 = mode;
      EX0 = 1;
      break;
    case 1:
      IT1 = mode;
      EX1 = 1;
      break;
    }
  }
}

void detachInterrupt(__data uint8_t interruptNum) {
  if (interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    // Disable the interrupt.  (We can't assume that interruptNum is equal
    // to the number of the EIMSK bit to clear, as this isn't true on the
    // ATmega8.  There, INT0 is 6 and INT1 is 7.)
    switch (interruptNum) {
    case 0:
      EX0 = 0;
      break;
    case 1:
      EX1 = 0;
      break;
    }
    intFunc[interruptNum] = nothing;
  }
}
