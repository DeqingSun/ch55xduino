/*
 created by Deqing Sun for use with CH55xduino
 */

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino_include.h"

#if defined(CH559)
uint16_t analogRead(uint8_t pin)
#else
uint8_t analogRead(uint8_t pin)
#endif
{
    pin = analogPinToChannel(pin);
    
    if (pin == NOT_ANALOG) return 0;

#if defined(CH551) || defined(CH552)
    ADC_CFG = bADC_EN | bADC_CLK;
    
    ADC_CTRL = (ADC_CTRL & ~(0x03)) | (0x03&pin);
    
    ADC_START = 1;
    
    while(ADC_START);
    
    return ADC_DATA;
#elif defined(CH559)
    
    uint8_t pinMask = 1<<pin;
    P1_IE &= ~(pinMask); //Close other data functions of P1 port, if only part of the sampling channel is used, set the rest to 1, otherwise it will affect the IO function
    ADC_SETUP |= bADC_POWER_EN; //ADC power enable
    ADC_CK_SE = (F_CPU/6000000L); //Set frequency division, make it similar to 6M
    ADC_CTRL &= ~MASK_ADC_CYCLE; //Select manual sampling
    ADC_CTRL &= ~(bADC_CHANN_MOD1 | bADC_CHANN_MOD0); //Manually select channel
    ADC_CHANN = pinMask; //Gate channel 1
    ADC_EX_SW |= bADC_RESOLUTION; //Sampling bits 11bit
    // ADC_EX_SW &= ~bADC_RESOLUTION; //Sampling bits 10bit
    delayMicroseconds(10); //Optional, wait for the channel to switch successfully
    ADC_CTRL |= bADC_SAMPLE; //Manually generate sampling pulse
    delayMicroseconds(5);
    ADC_CTRL &= ~bADC_SAMPLE;
    while((ADC_STAT & bADC_IF_ACT) == 0); //Non-interrupt mode, waiting for the completion of the acquisition
    ADC_STAT |= bADC_IF_ACT;
    uint16_t ADCValue = ADC_FIFO;
    return ADCValue; //Return sample value

#else
    return 0;
#endif
}


// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
void analogWrite(uint8_t pin, __xdata uint16_t val)
{
    // We need to make sure the PWM output is enabled for those pins
    // that support it, as we turn it off when digitally reading or
    // writing with them.  Also, make sure the pin is in output mode
    // for consistenty with Wiring, which doesn't require a pinMode
    // call for the analog output pins.
#if defined(CH551) || defined(CH552)
    pinMode(pin, OUTPUT);
    if (val == 0)
    {
        digitalWrite(pin, LOW);
    }
    else if (val >= 256)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        switch(digitalPinToPWM(pin))
        {
            case PIN_PWM1:
                PIN_FUNC &= ~(bPWM1_PIN_X);
                PWM_CTRL |= bPWM1_OUT_EN;
                PWM_DATA1 = val;
                break;
            case PIN_PWM2:
                PIN_FUNC &= ~(bPWM2_PIN_X);
                PWM_CTRL |= bPWM2_OUT_EN;
                PWM_DATA2 = val;
                break;
            case PIN_PWM1_:
                PIN_FUNC |= (bPWM1_PIN_X);
                PWM_CTRL |= bPWM1_OUT_EN;
                PWM_DATA1 = val;
                break;
            case PIN_PWM2_:
                PIN_FUNC |= (bPWM2_PIN_X);
                PWM_CTRL |= bPWM2_OUT_EN;
                PWM_DATA2 = val;
                break;
            case NOT_ON_PWM:
            default:
                if (val < 128) {
                    digitalWrite(pin, LOW);
                } else {
                    digitalWrite(pin, HIGH);
                }
        }
    }
#elif defined(CH559)
    pinMode(pin, OUTPUT);
    if (val == 0)
    {
        digitalWrite(pin, LOW);
    }
    else if (val >= 256)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        uint8_t pwmPin = digitalPinToPWM(pin);
        if (pwmPin!=NOT_ON_PWM){
#if (F_CPU/(1000L*256))>255
            PWM_CK_SE = 255;
#else
            PWM_CK_SE = (F_CPU/(1000L*256));
#endif
            PWM_CYCLE = 255;
        }
        switch(pwmPin)
        {
            case PIN_PWM1:
                PIN_FUNC &= ~(bPWM1_PIN_X); //CH559 only has 1 bit for 2 PWMs
                PWM_CTRL |= bPWM_OUT_EN;
                PWM_DATA = val;
                break;
            case PIN_PWM2:
                PIN_FUNC &= ~(bPWM1_PIN_X);
                PWM_CTRL |= bPWM2_OUT_EN;
                PWM_DATA2 = val;
                break;
            case PIN_PWM1_:
                PIN_FUNC |= (bPWM1_PIN_X);
                PWM_CTRL |= bPWM_OUT_EN;
                PWM_DATA = val;
                break;
            case PIN_PWM2_:
                PIN_FUNC |= (bPWM1_PIN_X);
                PWM_CTRL |= bPWM2_OUT_EN;
                PWM_DATA2 = val;
                break;
            case NOT_ON_PWM:
            default:
                if (val < 128) {
                    digitalWrite(pin, LOW);
                } else {
                    digitalWrite(pin, HIGH);
                }
        }
    }
#else
    return;
#endif
}

