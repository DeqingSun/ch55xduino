#!/bin/bash

#prepared for mac

COREVERSION=0.0.12
SDCCVERSION=13407
TOOLSVERSION=2022.05.06
COREFILE=ch55xduino-core-$COREVERSION.tar.bz2

#pack core

#Stop OS X tar from including hidden ._ files in archives
export COPYFILE_DISABLE=1;

tar -jcv --exclude="*DS_Store" -f packedFiles/$COREFILE -C ../ ch55x

./gen_platform_entry.sh packedFiles/$COREFILE $COREVERSION $SDCCVERSION $TOOLSVERSION > packedFiles/platform_$COREVERSION.txt

#pack tools
TOOLFILE_MAC=ch55xduino-tools_macosx-$TOOLSVERSION.tar.bz2
tar -jcv --exclude="*DS_Store" -f packedFiles/$TOOLFILE_MAC -C ../ tools/wrapper tools/macosx
TOOLFILE_LINUX=ch55xduino-tools_linux64-$TOOLSVERSION.tar.bz2
tar -jcv --exclude="*DS_Store" -f packedFiles/$TOOLFILE_LINUX -C ../ tools/wrapper tools/linux
TOOLFILE_WIN=ch55xduino-tools_mingw32-$TOOLSVERSION.tar.bz2
tar -jcv --exclude="*DS_Store" -f packedFiles/$TOOLFILE_WIN -C ../ tools/wrapper tools/win
./gen_tools_entry.sh packedFiles/ch55xduino-tools $TOOLSVERSION $COREVERSION > packedFiles/tools_$TOOLSVERSION.txt

./gen_tools_entry.sh sdcc $SDCCVERSION $COREVERSION > packedFiles/sdcc_$SDCCVERSION.txt
