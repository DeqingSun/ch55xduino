SPIFlash
========

CH55xduino library for read/write access to SPI flash memory chips. This works with 256byte/page SPI flash memory such as the 4MBIT W25X40CLSNIG and the W25Qxx variants.
For instance a 4MBit (512Kbyte) flash chip will have 2048 pages: 256*2048 = 524288 bytes (512Kbytes).
Minimal modifications should allow chips that have different page size to work.
DEPENDS ON: CH55xduino SPI library (present in this repository).


### Installation
Once the CH55xduino is installed on Arduino IDE, just access the 'Examples' menu and then SPIFlash

### License

Copyright (c) 2022 by Jonathas Barbosa
This library was based on Felix Rusu's work present in https://github.com/LowPowerLab/SPIFlash. All rights reserved.
<br/><br/>
Copyright (c) 2013-2018 by Felix Rusu <felix@lowpowerlab.com>
<br/><br/>
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
<br/><br/>
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
<br/><br/>
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
