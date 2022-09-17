// Copyright (c) 2022 - SPIFlash ported to CH55xduino by Jonathas Barbosa
//
// Copyright (c) 2013-2015 by Felix Rusu, LowPowerLab.com
// SPI Flash memory library for arduino/moteino.
// This works with 256byte/page SPI flash memory
// For instance a 4MBit (512Kbyte) flash chip will have 2048 pages: 256*2048 = 524288 bytes (512Kbytes)
// Minimal modifications should allow chips that have different page size but modifications
// DEPENDS ON: Arduino SPI library
// > Updated Jan. 5, 2015, TomWS1, modified writeBytes to allow blocks > 256 bytes and handle page misalignment.
// > Updated Feb. 26, 2015 TomWS1, added support for SPI Transactions (Arduino 1.5.8 and above)
// > Selective merge by Felix after testing in IDE 1.0.6, 1.6.4
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code

#include "SPIFlash.h"


//uint8_t SPIFlash::UNIQUEID[8];

/// IMPORTANT: NAND FLASH memory requires erase before write, because
///            it can only transition from 1s to 0s and only the erase command can reset all 0s to 1s
///            See http://en.wikipedia.org/wiki/Flash_memory
///            The smallest range that can be erased is a sector (4K, 32K, 64K); there is also a chip erase command

/// IMPORTANT: When flash chip is powered down, aka sleeping, the only command it will respond to is 
///            Release Power-down / Device ID (ABh), per section 8.2.19 of the W25X40CL datasheet.
///            This means after using the sleep() function of this library, wake() must be the first
///            function called. If other commands are used, the flash chip will ignore the commands. 


static void select() 
{
  //SPI_beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_PIN, LOW);
}

static void unselect() 
{
  digitalWrite(CS_PIN, HIGH);
  //SPI_endTransaction();   <- inexistent function in ch55xduino
}



/// setup SPI, read device ID etc...
boolean initialize()
{
  pinMode(CS_PIN, OUTPUT);
  
  SPI_begin();
  SPI_beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  unselect();
  wakeup();
  
  return true;
}

/// Get the manufacturer and device ID bytes (as a short word)
uint16_t readDeviceId()
{
  select();
  SPI_transfer(SPIFLASH_IDREAD);
  uint16_t jedecid = SPI_transfer(0) << 8;
  jedecid |= SPI_transfer(0);
  unselect();
  return jedecid;
}


/// Get the 64 bit unique identifier, stores it in UNIQUEID[8]. Only needs to be called once, ie after initialize
/// Returns the byte pointer to the UNIQUEID byte array
/// Read UNIQUEID like this:
/// flash.readUniqueId(); for (uint8_t i=0;i<8;i++) { Serial.print(flash.UNIQUEID[i], HEX); Serial.print(' '); }
/// or like this:
/// flash.readUniqueId(); uint8_t* MAC = flash.readUniqueId(); for (uint8_t i=0;i<8;i++) { Serial.print(MAC[i], HEX); Serial.print(' '); }
uint8_t* readUniqueId()
{
  command(SPIFLASH_MACREAD, false);
  SPI_transfer(0);
  SPI_transfer(0);
  SPI_transfer(0);
  SPI_transfer(0);
  for (uint8_t i=0;i<8;i++)
    UNIQUEID[i] = SPI_transfer(0);
  unselect();
    
  return UNIQUEID;
}

/// read 1 byte from flash memory
uint8_t readByte(uint32_t addr) {
  command(SPIFLASH_ARRAYREADLOWFREQ, false);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  uint8_t result = SPI_transfer(0);
  unselect();
  return result;
}

/// read unlimited # of bytes
void readBytes(uint32_t addr, void* buf, uint16_t len) {
  command(SPIFLASH_ARRAYREAD, false);
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  SPI_transfer(0); //"dont care"
  for (uint16_t i = 0; i < len; ++i)
    ((uint8_t*) buf)[i] = SPI_transfer(0);
  unselect();
}

/// Send a command to the flash chip, pass TRUE for isWrite when its a write command
void command(uint8_t cmd, boolean isWrite){
  if (isWrite)
  {
    command(SPIFLASH_WRITEENABLE, false); // Write Enable
    unselect();
  }
  //  wait for any write/erase to complete
  //  a time limit cannot really be added here without it being a very large safe limit
  //  that is because some chips can take several seconds to carry out a chip erase or other similar multi block or entire-chip operations
  //  
  //  Note: If the MISO line is high, busy() will return true. 
  //        This can be a problem and cause the code to hang when there is noise/static on MISO data line when:
  //        1) There is no flash chip connected
  //        2) The flash chip connected is powered down, aka sleeping. 
  if (cmd != SPIFLASH_WAKE) while(busy());
  select();
  SPI_transfer(cmd);
}



/// check if the chip is busy erasing/writing
boolean busy()
{
  return readStatus() & 1;
}


/// return the STATUS register
uint8_t readStatus()
{
  select();
  SPI_transfer(SPIFLASH_STATUSREAD);
  uint8_t status = SPI_transfer(0);
  unselect();
  return status;
}


/// Write 1 byte to flash memory
/// WARNING: you can only write to previously erased memory locations (see datasheet)
///          use the block erase commands to first clear memory (write 0xFFs)
void writeByte(uint32_t addr, uint8_t byt) {
  command(SPIFLASH_BYTEPAGEPROGRAM, true);  // Byte/Page Program
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  SPI_transfer(byt);
  unselect();
}

/// write multiple bytes to flash memory (up to 64K)
/// WARNING: you can only write to previously erased memory locations (see datasheet)
///          use the block erase commands to first clear memory (write 0xFFs)
/// This version handles both page alignment and data blocks larger than 256 bytes.
///
void writeBytes(uint32_t addr, const void* buf, uint16_t len) {
  uint16_t n;
  uint16_t maxBytes = 256-(addr%256);  // force the first set of bytes to stay within the first page
  uint16_t offset = 0;
  while (len>0)
  {
    n = (len<=maxBytes) ? len : maxBytes;
    command(SPIFLASH_BYTEPAGEPROGRAM, true);  // Byte/Page Program
    SPI_transfer(addr >> 16);
    SPI_transfer(addr >> 8);
    SPI_transfer(addr);
    
    for (uint16_t i = 0; i < n; i++)
      SPI_transfer(((uint8_t*) buf)[offset + i]);
    unselect();
    
    addr+=n;  // adjust the addresses and remaining bytes by what we've just transferred.
    offset +=n;
    len -= n;
    maxBytes = 256;   // now we can do up to 256 bytes per loop
  }
}

/// erase entire flash memory array
/// may take several seconds depending on size, but is non blocking
/// so you may wait for this to complete using busy() or continue doing
/// other things and later check if the chip is done with busy()
/// note that any command will first wait for chip to become available using busy()
/// so no need to do that twice
void chipErase() {
  command(SPIFLASH_CHIPERASE, true);
  unselect();
}
/// erase a 4Kbyte block
void blockErase4K(uint32_t addr) {
  command(SPIFLASH_BLOCKERASE_4K, true); // Block Erase
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  unselect();
}

/// erase a 32Kbyte block
void blockErase32K(uint32_t addr) {
  command(SPIFLASH_BLOCKERASE_32K, true); // Block Erase
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  unselect();
}

/// erase a 64Kbyte block
void blockErase64K(uint32_t addr) {
  command(SPIFLASH_BLOCKERASE_64K, true); // Block Erase
  SPI_transfer(addr >> 16);
  SPI_transfer(addr >> 8);
  SPI_transfer(addr);
  unselect();
}

/// Put flash memory chip into power down mode
/// WARNING: after this command, only the WAKEUP and DEVICE_ID commands are recognized
/// hence a wakeup() command should be invoked first before further operations
/// If a MCU soft restart is possible with flash chip left in sleep(), then a wakeup() command
///   should always be invoked before any other commands to ensure the flash chip was not left in sleep
void sleep() {
  command(SPIFLASH_SLEEP, false);
  unselect();
}

/// Wake flash memory from power down mode
/// NOTE: this command is required after a sleep() command is used, or no other commands will be recognized
/// If a MCU soft restart is possible with flash chip left in sleep(), then a wakeup() command
///   should always be invoked before any other commands to ensure the flash chip was not left in sleep
void wakeup() {
  command(SPIFLASH_WAKE, false);
  unselect();
}

/// cleanup
void end() {
  SPI_end();
}