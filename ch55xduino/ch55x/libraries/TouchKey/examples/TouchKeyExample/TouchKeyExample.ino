#include <TouchKey.h>

#define LED_BUILTIN 33

uint8_t prevSensorValue = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  TouchKey_begin( (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) ); //Enable all 6 channels: TIN0(P1.0), TIN1(P1.1), TIN2(P1.4), TIN3(P1.5), TIN4(P1.6), TIN5(P1.7)

  // In most cases you don't need to adjust parameters. But if you do, test with TouchKeyTuneParameter example
  /*
    //refer to AN3891 MPR121 Baseline System for more details
    TouchKey_SetMaxHalfDelta(5);      //increase if sensor value are more noisy
    TouchKey_SetNoiseHalfDelta(2);    //If baseline need to adjust at higher rate, increase this value
    TouchKey_SetNoiseCountLimit(10);  //If baseline need to adjust faster, increase this value
    TouchKey_SetFilterDelayLimit(5);  //make overall adjustment slopwer

    TouchKey_SetTouchThreshold(100);  //Bigger touch pad can use a bigger value
    TouchKey_SetReleaseThreshold(80); //Smaller than touch threshold
  */
}

void loop() {
  // put your main code here, to run repeatedly:
  TouchKey_Process();
  uint8_t touchResult = TouchKey_Get();
  if (touchResult) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
  for (uint8_t i = 0; i < 6; i++) {
    USBSerial_print("TIN");
    USBSerial_print(i);
    if (touchResult & (1 << i)) {
      USBSerial_print(" X ");
    } else {
      USBSerial_print(" _ ");
    }
  }
  USBSerial_println();

}
