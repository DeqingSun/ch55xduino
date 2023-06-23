/*
 created by Deqing Sun for use with CH55xduino
 need SDCC 13402 or higher version
 */

#include "Arduino.h"

void printNothing() { // only appear in this file
}

void USBSerial_print_i_func(__data long i) {
  Print_print_i(USBSerial_write, i);
}
void USBSerial_print_ib_func(__data long i, __xdata uint8_t base) {
  Print_print_ib(USBSerial_write, i, base);
}
void USBSerial_print_u_func(__data unsigned long u) {
  Print_print_u(USBSerial_write, u);
}
void USBSerial_print_ub_func(__data unsigned long u, __xdata uint8_t base) {
  Print_print_ib(USBSerial_write, u, base);
}
void USBSerial_print_s_func(char *__data s) {
  Print_print_s(USBSerial_write, s);
}
void USBSerial_print_sn_func(char *__data s, __xdata uint8_t size) {
  Print_print_sn(USBSerial_write, s, size);
}
