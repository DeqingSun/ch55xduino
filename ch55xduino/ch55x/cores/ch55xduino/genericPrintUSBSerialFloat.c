/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 float has a separate file to avoid unnecessary linking
 */

#include "Arduino.h"

void USBSerial_print_f_func(float f) {
    Print_print_f(USBSerial_write, f);
}
