#!/bin/bash

#using avr-gcc 7.3.0-atmel3.6.1-arduino

if [ $# -lt 2 ]; then
	echo "Add avr_objcopy to existing bz2 package"
	echo
	echo "usage: $0 sdcc-snapshot-filename avr_objcopy-filename"
	exit 1
fi

VERBOSE=

FILE=$(realpath $1)
OBJCOPYFILE=$(realpath $2)

NAME=$(basename "$FILE")

# remove all suffixes
NAME=${NAME%%.t*}
NAME=${NAME%%.z*}

TMP=$(mktemp -d sdcc-repack-XXXXXX)
echo "Unpacking into $TMP..."

tar x${VERBOSE}jf "$FILE" -C "$TMP"

[ -d "$TMP/sdcc/bin" ] && cp $OBJCOPYFILE $TMP/sdcc/bin

echo "Repacking into file $NAME"
#Stop OS X tar from including hidden ._ files in archives
export COPYFILE_DISABLE=1;
tar -jc${VERBOSE} --exclude="*DS_Store" -f "added/${NAME}_2.tar.bz2" -C "$TMP" sdcc

echo "cleaning up temporary files"
rm -rf "$TMP"

echo "done."