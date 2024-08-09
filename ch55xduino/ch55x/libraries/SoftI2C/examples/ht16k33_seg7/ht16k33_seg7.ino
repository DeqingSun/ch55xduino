#include <SoftI2C.h>

//tested on Adafruit 4-Digit 7-Segment LED Matrix Display FeatherWing
#define HT16K33_ADDR 0x70
const uint8_t sevensegfonttable[];

void setup() {
  //pass SCL and SDA
  Wire_begin(30, 31);
  // Initialize the display
  uint8_t init_data[1];
  // Turn on oscillator
  init_data[0] = 0x21;
  Wire_writeBytes(HT16K33_ADDR, init_data, 1);
  // Turn brightness to max
  init_data[0] = 0xEF;
  Wire_writeBytes(HT16K33_ADDR, init_data, 1);
  // Turn display on
  init_data[0] = 0x81;
  Wire_writeBytes(HT16K33_ADDR, init_data, 1);
}

void loop() {
  // Display milliseconds
  uint16_t ms = millis();
  uint8_t digit1 = (ms / 1000) % 10;
  ms = ms % 1000;
  uint8_t digit2 = ms / 100;
  ms = ms % 100;
  uint8_t digit3 = ms / 10;
  ms = ms % 10;
  uint8_t digit4 = ms;

  uint8_t displayData[11];
  memset(displayData, 0, 11);
  displayData[0] = 0x00;
  displayData[1] = sevensegfonttable[digit1 + '0' - 32];
  displayData[3] = sevensegfonttable[digit2 + '0' - 32];
  displayData[7] = sevensegfonttable[digit3 + '0' - 32];
  displayData[9] = sevensegfonttable[digit4 + '0' - 32];
  Wire_writeBytes(HT16K33_ADDR, displayData, 11);
}


const uint8_t sevensegfonttable[] = {

  0b00000000,  // (space)
  0b10000110,  // !
  0b00100010,  // "
  0b01111110,  // #
  0b01101101,  // $
  0b11010010,  // %
  0b01000110,  // &
  0b00100000,  // '
  0b00101001,  // (
  0b00001011,  // )
  0b00100001,  // *
  0b01110000,  // +
  0b00010000,  // ,
  0b01000000,  // -
  0b10000000,  // .
  0b01010010,  // /
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111,  // 9
  0b00001001,  // :
  0b00001101,  // ;
  0b01100001,  // <
  0b01001000,  // =
  0b01000011,  // >
  0b11010011,  // ?
  0b01011111,  // @
  0b01110111,  // A
  0b01111100,  // B
  0b00111001,  // C
  0b01011110,  // D
  0b01111001,  // E
  0b01110001,  // F
  0b00111101,  // G
  0b01110110,  // H
  0b00110000,  // I
  0b00011110,  // J
  0b01110101,  // K
  0b00111000,  // L
  0b00010101,  // M
  0b00110111,  // N
  0b00111111,  // O
  0b01110011,  // P
  0b01101011,  // Q
  0b00110011,  // R
  0b01101101,  // S
  0b01111000,  // T
  0b00111110,  // U
  0b00111110,  // V
  0b00101010,  // W
  0b01110110,  // X
  0b01101110,  // Y
  0b01011011,  // Z
  0b00111001,  // [
  0b01100100,  //
  0b00001111,  // ]
  0b00100011,  // ^
  0b00001000,  // _
  0b00000010,  // `
  0b01011111,  // a
  0b01111100,  // b
  0b01011000,  // c
  0b01011110,  // d
  0b01111011,  // e
  0b01110001,  // f
  0b01101111,  // g
  0b01110100,  // h
  0b00010000,  // i
  0b00001100,  // j
  0b01110101,  // k
  0b00110000,  // l
  0b00010100,  // m
  0b01010100,  // n
  0b01011100,  // o
  0b01110011,  // p
  0b01100111,  // q
  0b01010000,  // r
  0b01101101,  // s
  0b01111000,  // t
  0b00011100,  // u
  0b00011100,  // v
  0b00010100,  // w
  0b01110110,  // x
  0b01101110,  // y
  0b01011011,  // z
  0b01000110,  // {
  0b00110000,  // |
  0b01110000,  // }
  0b00000001,  // ~
  0b00000000,  // del
};
