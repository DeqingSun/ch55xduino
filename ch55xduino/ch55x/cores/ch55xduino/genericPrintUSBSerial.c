/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

#include "Arduino.h"

void USBSerial_print_i_func(long i) {
    Print_print_i(USBSerial_write, i);
}
void USBSerial_print_u_func(unsigned long u) {
    Print_print_u(USBSerial_write, u);
}
void USBSerial_print_s_func(char * s) {
    Print_print_s(USBSerial_write, s);
}
