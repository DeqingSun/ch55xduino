/*
 *
 * SUMP Protocol Implementation for CH55x boards.
 *
 * Copyright (c) 2011,2012,2013,2014,2015 Andrew Gillham
 * Copyright (c) 2020 Deqing Sun
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREW GILLHAM ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANDREW GILLHAM BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

/*
 *
 * This Arduino sketch implements a SUMP protocol compatible with the standard
 * SUMP client as well as the alternative client from here:
 *  https://sigrok.org/wiki/PulseView
 *
 * This SUMP protocol compatible logic analyzer for the CH55x board supports
 * 4 channels consisting of digital pins P14 to P17 (CH0~CH3)
 *
 * To use this with the original or alternative SUMP clients,
 * use these settings:
 *
 * Sampling rate: 5MHz (or lower)
 * Channel Groups: 0 (zero) only
 * Recording Size: 1480 (740*2)
 * Noise Filter: doesn't matter
 * RLE: disabled (unchecked)
 *
 * Tested with PulseView 0.4.2 and ch552
 *
 * Release: v0.1 August 27, 2020.
 *
 */

/*
   Function prototypes so this can compile from the cli.
   You'll need the 'arduino-core' package and to check the paths in the
   Makefile.
*/

void getCmd(void);
void get_metadata(void);
void captureInline5mhz(void);
void captureInlineWithT2(void);

#define LED_BUILTIN 33

/* XON/XOFF are not supported. */
#define SUMP_RESET 0x00
#define SUMP_ARM   0x01
#define SUMP_QUERY 0x02
#define SUMP_RETURN_CAPTURE_DATA  0x08
#define SUMP_XON   0x11
#define SUMP_XOFF  0x13

/* mask & values used, config ignored. only stage0 supported */
#define SUMP_TRIGGER_MASK 0xC0
#define SUMP_TRIGGER_VALUES 0xC1
#define SUMP_TRIGGER_CONFIG 0xC2

#define SUMP_TRIGGER_MASK_2 0xC4
#define SUMP_TRIGGER_MASK_3 0xC8
#define SUMP_TRIGGER_MASK_4 0xCC
#define SUMP_TRIGGER_VALUES_2 0xC5
#define SUMP_TRIGGER_VALUES_3 0xC9
#define SUMP_TRIGGER_VALUES_4 0xCD
#define SUMP_TRIGGER_CONFIG_2 0xC6
#define SUMP_TRIGGER_CONFIG_3 0xCA
#define SUMP_TRIGGER_CONFIG_4 0xCE

/* Most flags (except RLE) are ignored. */
#define SUMP_SET_DIVIDER 0x80
#define SUMP_SET_READ_DELAY_COUNT 0x81
#define SUMP_SET_FLAGS 0x82
#define SUMP_SET_RLE 0x0100

/* extended commands -- self-test unsupported, but metadata is returned. */
#define SUMP_SELF_TEST 0x03
#define SUMP_GET_METADATA 0x04

#define MAX_CAPTURE_SIZE (740*2)

// SUMP command from host (via serial)
// SUMP commands are either 1 byte, or for the extended commands, 5 bytes.

uint8_t cmdByte = 0;
uint8_t cmdBytes[5];


__xdata uint8_t logicdata[MAX_CAPTURE_SIZE / 2];
unsigned int readCount = MAX_CAPTURE_SIZE;
unsigned int delayCount = 0;
uint8_t trigger = 0;
uint8_t trigger_values = 0;
unsigned long divider = 0;
boolean rleEnabled = 0;

__idata uint8_t swapByte; //for XCHD instruction



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}


void loop()
{
  int i;

  if (USBSerial_available() > 0) {
    cmdByte = USBSerial_read();
    switch (cmdByte) {
      case SUMP_RESET:  //0x00
        //   We don't do anything here as some unsupported extended commands have
        //   zero bytes and are mistaken as resets.  This can trigger false resets
        //   so we don't erase the data or do anything for a reset.
        break;
      case SUMP_QUERY:  //0x02
        // return the expected bytes.
        USBSerial_write('1');
        USBSerial_write('A');
        USBSerial_write('L');
        USBSerial_write('S');
        break;
      case SUMP_ARM:    //0x01
        // Zero out any previous samples before arming.
        // Done here instead via reset due to spurious resets.

        for (i = 0 ; i < (sizeof(logicdata) / sizeof(logicdata[0])); i++) {
          logicdata[i] = 0;
        }

        digitalWrite(LED_BUILTIN, HIGH);
        if (divider <= 19) {
          captureInline5mhz();
        } else {
          // 2.0MHz+
          captureInlineWithT2();
        }
        digitalWrite(LED_BUILTIN, LOW);
        break;

      case SUMP_RETURN_CAPTURE_DATA:  //0x08
        break;

      case SUMP_TRIGGER_MASK:  //0xC0
        //  the trigger mask byte has a '1' for each enabled trigger so
        //   we can just use it directly as our trigger mask.
        getCmd();

        trigger = cmdBytes[0] << 4; //we use P14~P17

        break;
      case SUMP_TRIGGER_VALUES:  //0xC1
        // trigger_values can be used directly as the value of each bit
        // defines whether we're looking for it to be high or low.
        getCmd();
        trigger_values = cmdBytes[0] << 4;

        break;
      case SUMP_TRIGGER_CONFIG:  //0xC1
        // read the rest of the command bytes, but ignore them.
        getCmd();
        break;
      case SUMP_SET_DIVIDER: //0x80
        // the shifting needs to be done on the 32bit unsigned long variable
        // so that << 16 doesn't end up as zero.
        // f = clock / (x + 1), f seems to be 100M
        getCmd();
        divider = cmdBytes[2];
        divider = divider << 8;
        divider += cmdBytes[1];
        divider = divider << 8;
        divider += cmdBytes[0];

        break;
      case SUMP_SET_READ_DELAY_COUNT:
        /*
             this just sets up how many samples there should be before
           and after the trigger fires.  The readCount is total samples
           to return and delayCount number of samples after the trigger.
           this sets the buffer splits like 0/100, 25/75, 50/50
           for example if readCount == delayCount then we should
           return all samples starting from the trigger point.
           if delayCount < readCount we return (readCount - delayCount) of
           samples from before the trigger fired.
        */
        getCmd();
        readCount = 4 * (((cmdBytes[1] << 8) | cmdBytes[0]) + 1);
        if (readCount > MAX_CAPTURE_SIZE)
          readCount = MAX_CAPTURE_SIZE;
        delayCount = 4 * (((cmdBytes[3] << 8) | cmdBytes[2]) + 1);
        if (delayCount > MAX_CAPTURE_SIZE)
          delayCount = MAX_CAPTURE_SIZE;

        break;
      case SUMP_TRIGGER_MASK_2:
      case SUMP_TRIGGER_MASK_3:
      case SUMP_TRIGGER_MASK_4:
      case SUMP_TRIGGER_VALUES_2:
      case SUMP_TRIGGER_VALUES_3:
      case SUMP_TRIGGER_VALUES_4:
      case SUMP_TRIGGER_CONFIG_2:
      case SUMP_TRIGGER_CONFIG_3:
      case SUMP_TRIGGER_CONFIG_4:
        getCmd();
        //ignore
        break;

      case SUMP_SET_FLAGS:
        /* read the rest of the command bytes and check if RLE is enabled. */
        getCmd();
        rleEnabled = ((cmdBytes[1] & 0b1000000) != 0);

        break;

      case SUMP_GET_METADATA:
        // We return a description of our capabilities.
        // Check the function's comments below.
        get_metadata();
        break;
      case SUMP_SELF_TEST:
        /* ignored. */
        break;
      default:
        // ignore any unrecognized bytes.
        break;
    }
  }
}


/*
   Extended SUMP commands are 5 bytes.  A command byte followed by 4 bytes
   of options. We already read the command byte, this gets the remaining
   4 bytes of the command.
*/
void getCmd() {
  delay(1);  
  cmdBytes[0] = USBSerial_read();
  cmdBytes[1] = USBSerial_read();
  cmdBytes[2] = USBSerial_read();
  cmdBytes[3] = USBSerial_read();
}

/*
   This function returns the metadata about our capabilities.  It is sent in
   response to the  OpenBench Logic Sniffer extended get metadata command.
*/
void get_metadata() {
  // device name
  USBSerial_write((uint8_t)0x01);
  USBSerial_write('A');
  USBSerial_write('G');
  USBSerial_write('L');
  USBSerial_write('A');
  USBSerial_write(' ');
  USBSerial_write('C');
  USBSerial_write('H');
  USBSerial_write('5');
  USBSerial_write('5');
  USBSerial_write('x');
  USBSerial_write(' ');
  USBSerial_write('v');
  USBSerial_write('0');
  USBSerial_write((uint8_t)0x00);

  // firmware version
  USBSerial_write((uint8_t)0x02);
  USBSerial_write('0');
  USBSerial_write('.');
  USBSerial_write('1');
  USBSerial_write('3');
  USBSerial_write((uint8_t)0x00);

  // sample memory
  USBSerial_write((uint8_t)0x21);
  USBSerial_write((uint8_t)0x00);
  USBSerial_write((uint8_t)0x00);
  // 740*2 = 1480 bytes
  USBSerial_write((uint8_t)0x05);
  USBSerial_write((uint8_t)0xC8);

  // sample rate (5MHz)
  USBSerial_write((uint8_t)0x23);
  USBSerial_write((uint8_t)0x00);
  USBSerial_write((uint8_t)0x4C);
  USBSerial_write((uint8_t)0x4B);
  USBSerial_write((uint8_t)0x40);

  // number of probes (4 for ch55x)
  USBSerial_write((uint8_t)0x40);
  USBSerial_write((uint8_t)0x04);

  // protocol version (2)
  USBSerial_write((uint8_t)0x41);
  USBSerial_write((uint8_t)0x02);

  // end of data
  USBSerial_write((uint8_t)0x00);
}
