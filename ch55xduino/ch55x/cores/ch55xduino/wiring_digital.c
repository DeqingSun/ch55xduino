/*
 created by Deqing Sun for use with CH55xduino
 */

#define ARDUINO_MAIN

// clang-format off
#include "wiring_private.h"
#include "pins_arduino_include.h"

#include "pins_arduino.h"   //only include once in core
// clang-format on

void pinMode(__data uint8_t pin,
             __xdata uint8_t mode) // only P1 & P3 can set mode
{
  //__data make sure the local varaibles reside in register in large model
  __data uint8_t bit = digitalPinToBitMask(pin);
  __data uint8_t port = digitalPinToPort(pin);

  if (port == NOT_A_PIN)
    return;

  if (mode == INPUT) {
#if defined(CH551) || defined(CH552) || defined(CH549)
    if (port == P1PORT) {
      P1_MOD_OC &= ~bit;
      P1_DIR_PU &= ~bit;
    } else if (port == P3PORT) {
      P3_MOD_OC &= ~bit;
      P3_DIR_PU &= ~bit;
    }
#endif
#if defined(CH549)
    else if (port == P0PORT) {
      P0_MOD_OC &= ~bit;
      P0_DIR_PU &= ~bit;
    } else if (port == P2PORT) {
      P2_MOD_OC &= ~bit;
      P2_DIR_PU &= ~bit;
    } else if (port == P4PORT) {
      P4_MOD_OC &= ~bit;
      P4_DIR_PU &= ~bit;
    }
#endif
#if defined(CH559) || defined(CH558)
    if (port == P0PORT) {
      PORT_CFG &= ~bP0_OC;
      P0_PU &= ~bit;
      P0_DIR &= ~bit;
    } else if (port == P1PORT) {
      P1_IE |= bit;
      PORT_CFG &= ~bP1_OC;
      P1_PU &= ~bit;
      P1_DIR &= ~bit;
    } else if (port == P2PORT) {
      PORT_CFG &= ~bP2_OC;
      P2_PU &= ~bit;
      P2_DIR &= ~bit;
    } else if (port == P3PORT) {
      PORT_CFG &= ~bP3_OC;
      P3_PU &= ~bit;
      P3_DIR &= ~bit;
    } else if (port == P4PORT) { // P4 can not do OC
      P4_PU &= ~bit;
      P4_DIR &= ~bit;
    } // P5 can only do input
#endif
  } else if (mode == INPUT_PULLUP) {
#if defined(CH551) || defined(CH552) || defined(CH549)
    if (port == P1PORT) {
      P1_MOD_OC |= bit;
      P1_DIR_PU |= bit;
    } else if (port == P3PORT) {
      P3_MOD_OC |= bit;
      P3_DIR_PU |= bit;
    }
#endif
#if defined(CH549)
    else if (port == P0PORT) {
      P0_MOD_OC |= bit;
      P0_DIR_PU |= bit;
    } else if (port == P2PORT) {
      P2_MOD_OC |= bit;
      P2_DIR_PU |= bit;
    } else if (port == P4PORT) {
      P4_MOD_OC |= bit;
      P4_DIR_PU |= bit;
    }
#endif
#if defined(CH559) || defined(CH558)
    if (port == P0PORT) {
      PORT_CFG &= ~bP0_OC;
      P0_PU |= bit;
      P0_DIR &= ~bit;
    } else if (port == P1PORT) {
      P1_IE |= bit;
      PORT_CFG &= ~bP1_OC;
      P1_PU |= bit;
      P1_DIR &= ~bit;
    } else if (port == P2PORT) {
      PORT_CFG &= ~bP2_OC;
      P2_PU |= bit;
      P2_DIR &= ~bit;
    } else if (port == P3PORT) {
      PORT_CFG &= ~bP3_OC;
      P3_PU |= bit;
      P3_DIR &= ~bit;
    } else if (port == P4PORT) { // P4 can not do OC
      P4_PU |= bit;
      P4_DIR &= ~bit;
    } // P5 can only do input
#endif
  } else if (mode == OUTPUT) {
#if defined(CH551) || defined(CH552) || defined(CH549)
    if (port == P1PORT) {
      P1_MOD_OC &= ~bit;
      P1_DIR_PU |= bit;
    } else if (port == P3PORT) {
      P3_MOD_OC &= ~bit;
      P3_DIR_PU |= bit;
    }
#endif
#if defined(CH549)
    else if (port == P0PORT) {
      P0_MOD_OC &= ~bit;
      P0_DIR_PU |= bit;
    } else if (port == P2PORT) {
      P2_MOD_OC &= ~bit;
      P2_DIR_PU |= bit;
    } else if (port == P4PORT) {
      P4_MOD_OC &= ~bit;
      P4_DIR_PU |= bit;
    }
#endif
#if defined(CH559) || defined(CH558)
    if (port == P0PORT) {
      PORT_CFG &= ~bP0_OC;
      P0_DIR |= bit;
    } else if (port == P1PORT) {
      PORT_CFG &= ~bP1_OC;
      P1_DIR |= bit;
    } else if (port == P2PORT) {
      PORT_CFG &= ~bP2_OC;
      P2_DIR |= bit;
    } else if (port == P3PORT) {
      PORT_CFG &= ~bP3_OC;
      P3_DIR |= bit;
    } else if (port == P4PORT) { // P4 can not do OC
      P4_DIR |= bit;
    } // P5 can only do input
#endif
  } else if (mode == OUTPUT_OD) {
#if defined(CH551) || defined(CH552) || defined(CH549)
    if (port == P1PORT) {
      P1_MOD_OC |= bit;
      P1_DIR_PU &= ~bit;
    } else if (port == P3PORT) {
      P3_MOD_OC |= bit;
      P3_DIR_PU &= ~bit;
    }
#endif
#if defined(CH549)
    else if (port == P0PORT) {
      P0_MOD_OC |= bit;
      P0_DIR_PU &= ~bit;
    } else if (port == P2PORT) {
      P2_MOD_OC |= bit;
      P2_DIR_PU &= ~bit;
    } else if (port == P4PORT) {
      P4_MOD_OC |= bit;
      P4_DIR_PU &= ~bit;
    }
#endif
#if defined(CH559) || defined(CH558)
    // open drain settings - see table 10.2.2 on page 28.
    // Selected as "High-impedance input weak standard bi-directional mode with open drain output, pins without pull-up resistor."
    // Ports P0-P3 support pure input, push-pull output and standard bi-direction modes, P4 supports pure input and push-pull output modes. 
    if (port == P0PORT) {
      PORT_CFG |= bP0_OC; // clear bP0_OC bit to get open drain and bi-dir modes.
      P0_DIR &= ~bit;
      P0_PU &= ~bit; 
    } else if (port == P1PORT) {
      PORT_CFG |= bP1_OC;
      P1_DIR |= bit;
      P1_PU &= ~bit; 
    } else if (port == P2PORT) {
      PORT_CFG |= bP2_OC;
      P2_DIR |= bit;
      P2_PU &= ~bit; 
    } else if (port == P3PORT) {
      PORT_CFG |= bP3_OC;
      P3_DIR |= bit;
      P3_PU &= ~bit; 
    } else if (port == P4PORT) {
      P4_DIR |= bit; // Open Drain not supported; fall back to normal output.
    }
#endif
  } // End of Open Drain mode.
}

static void turnOffPWM(__data uint8_t pwm) {
#if defined(CH551) || defined(CH552)
  switch (pwm) {
  case PIN_PWM1:
    if ((PIN_FUNC & bPWM1_PIN_X) == 0) {
      PWM_CTRL &= ~bPWM1_OUT_EN;
    }
    break;
  case PIN_PWM2:
    if ((PIN_FUNC & bPWM2_PIN_X) == 0) {
      PWM_CTRL &= ~bPWM2_OUT_EN;
    }
    break;
  case PIN_PWM1_:
    if ((PIN_FUNC & bPWM1_PIN_X) != 0) {
      PWM_CTRL &= ~bPWM1_OUT_EN;
    }
    break;
  case PIN_PWM2_:
    if ((PIN_FUNC & bPWM2_PIN_X) != 0) {
      PWM_CTRL &= ~bPWM2_OUT_EN;
    }
    break;
  }
#elif defined(CH559) | defined(CH558)
  switch (pwm) {
#if defined(CH559)  // CH558 only has one PWM(3)
  case PIN_PWM1:
    if ((PIN_FUNC & bPWM1_PIN_X) == 0) {
      PWM_CTRL &= ~bPWM_OUT_EN;
    }
    break;
  case PIN_PWM2:
    if ((PIN_FUNC & bPWM1_PIN_X) == 0) {
      PWM_CTRL &= ~bPWM2_OUT_EN;
    }
    break;
  case PIN_PWM1_:
    if ((PIN_FUNC & bPWM1_PIN_X) != 0) {
      PWM_CTRL &= ~bPWM_OUT_EN;
    }
    break;
  case PIN_PWM2_:
    if ((PIN_FUNC & bPWM1_PIN_X) != 0) {
      PWM_CTRL &= ~bPWM2_OUT_EN;
    }
    break;
#endif
  case PIN_PWM3:
    if ((PIN_FUNC & bTMR3_PIN_X) == 0) {
      if (T3_CTRL & bT3_OUT_EN) {
        T3_CTRL &= ~(bT3_OUT_EN | bT3_CNT_EN);
      }
    }
    break;
  case PIN_PWM3_:
    if ((PIN_FUNC & bTMR3_PIN_X) != 0) {
      if (T3_CTRL & bT3_OUT_EN) {
        T3_CTRL &= ~(bT3_OUT_EN | bT3_CNT_EN);
      }
    }
    break;
  }
#else
  pwm;
  return;
#endif
  // todo: PWM mode for CH559 and CHA558
}

uint8_t digitalRead(__data uint8_t pin) {
  __data uint8_t pwm = digitalPinToPWM(pin);
  __data uint8_t bit = digitalPinToBitMask(pin);
  __data uint8_t port = digitalPinToPort(pin);

  if (port == NOT_A_PIN)
    return LOW;

  // If the pin that support PWM output, we need to turn it off
  // before getting a digital reading.
  if (pwm != NOT_ON_PWM)
    turnOffPWM(pwm);

  __data uint8_t portBuf = 0;

  switch (port) {
#if defined(CH551) || defined(CH552) || defined(CH549) || defined(CH559) || defined(CH558)
  case P1PORT:
    portBuf = P1;
    break;
  case P2PORT:
    portBuf = P2;
    break;
  case P3PORT:
    portBuf = P3;
    break;
#endif
#if defined(CH549)
  case P0PORT:
    portBuf = P0;
    break;
  case P4PORT:
    portBuf = P4;
    break;
  case P5PORT:
    portBuf = P5;
    break;
#elif defined(CH559) || defined(CH558)
  case P0PORT:
    portBuf = P0;
    break;
  case P4PORT:
    portBuf = P4_IN;
    break;
  case P5PORT:
    portBuf = P5_IN;
    break;
#endif
  default:
    break;
  }

  if (portBuf & bit)
    return HIGH;
  return LOW;
}

void digitalWrite(__data uint8_t pin, __xdata uint8_t val) {
  __data uint8_t pwm = digitalPinToPWM(pin);
  __data uint8_t bit = digitalPinToBitMask(pin);
  __data uint8_t port = digitalPinToPort(pin);

  // If the pin that support PWM output, we need to turn it off
  // before doing a digital write.
  if (pwm != NOT_ON_PWM)
    turnOffPWM(pwm);

  // C pointers cannot be used to access the 8051's SFRs (special function
  // registers).

  __data uint8_t interruptOn = EA;
  EA = 0;

  switch (port) {
#if defined(CH551) || defined(CH552) || defined(CH549) || defined(CH559) || defined(CH558)
  case P1PORT:
    if (val == LOW) {
      P1 &= ~bit;
    } else {
      P1 |= bit;
    }
    break;
  case P2PORT:
    if (val == LOW) {
      P2 &= ~bit;
    } else {
      P2 |= bit;
    }
    break;
  case P3PORT:
    if (val == LOW) {
      P3 &= ~bit;
    } else {
      P3 |= bit;
    }
    break;
#endif
#if defined(CH549)
  case P0PORT:
    if (val == LOW) {
      P0 &= ~bit;
    } else {
      P0 |= bit;
    }
    break;
  case P4PORT:
    if (val == LOW) {
      P4 &= ~bit;
    } else {
      P4 |= bit;
    }
    break;
  case P5PORT:
    if (val == LOW) {
      P5 &= ~bit;
    } else {
      P5 |= bit;
    }
    break;
#elif defined(CH559) || defined(CH558)
  case P0PORT:
    if (val == LOW) {
      P0 &= ~bit;
    } else {
      P0 |= bit;
    }
    break;
  case P4PORT:
    if (val == LOW) {
      P4_OUT &= ~bit;
    } else {
      P4_OUT |= bit;
    }
    break;
#endif

  default:
    break;
  }

  if (interruptOn)
    EA = 1;
}
