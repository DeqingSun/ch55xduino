#include "osDetection.h"
#include <Arduino.h>

struct setups_data_t {
  uint8_t count;
  uint8_t cnt_02;
  uint8_t cnt_04;
  uint8_t cnt_ff;
  uint16_t last_wlength;
  os_variant_t detected_os;
};

__xdata struct setups_data_t setups_data = {
    .count = 0,
    .cnt_02 = 0,
    .cnt_04 = 0,
    .cnt_ff = 0,
    .detected_os = OS_UNSURE,
};

// Some collected sequences of wLength can be found in tests.
void make_guess(void) {
  if (setups_data.count < 3) {
    return;
  }
  if (setups_data.cnt_ff >= 2 && setups_data.cnt_04 >= 1) {
    setups_data.detected_os = OS_WINDOWS;
    return;
  }
  if (setups_data.count == setups_data.cnt_ff) {
    // Linux has 3 packets with 0xFF.
    setups_data.detected_os = OS_LINUX;
    return;
  }
  if (setups_data.count == 5 && setups_data.last_wlength == 0xFF &&
      setups_data.cnt_ff == 1 && setups_data.cnt_02 == 2) {
    setups_data.detected_os = OS_MACOS;
    return;
  }
  if (setups_data.count == 4 && setups_data.cnt_ff == 0 &&
      setups_data.cnt_02 == 2) {
    // iOS and iPadOS don't have the last 0xFF packet.
    setups_data.detected_os = OS_IOS;
    return;
  }
  if (setups_data.cnt_ff == 0 && setups_data.cnt_02 == 3 &&
      setups_data.cnt_04 == 1) {
    // This is actually PS5.
    setups_data.detected_os = OS_LINUX;
    return;
  }
  if (setups_data.cnt_ff >= 1 && setups_data.cnt_02 == 0 &&
      setups_data.cnt_04 == 0) {
    // This is actually Quest 2 or Nintendo Switch.
    setups_data.detected_os = OS_LINUX;
    return;
  }
}

void OS_detect_process_wlength(__data uint16_t w_length) {
  setups_data.count++;
  setups_data.last_wlength = w_length;
  if (w_length == 0x2) {
    setups_data.cnt_02++;
  } else if (w_length == 0x4) {
    setups_data.cnt_04++;
  } else if (w_length == 0xFF) {
    setups_data.cnt_ff++;
  }
  make_guess();
}

os_variant_t detected_host_os(void) {
  // Serial0_println("debug");
  // Serial0_print("02:");Serial0_println((int)setups_data.cnt_02);
  // Serial0_print("04:");Serial0_println((int)setups_data.cnt_04);
  // Serial0_print("ff:");Serial0_println((int)setups_data.cnt_ff);

  // on some initial test, the windows 11 and win7 virtual machine got reported
  // as linux, macbook reported as ios. But that should be fine for now
  return setups_data.detected_os;
}

void erase_wlength_data(void) { memset(&setups_data, 0, sizeof(setups_data)); }
