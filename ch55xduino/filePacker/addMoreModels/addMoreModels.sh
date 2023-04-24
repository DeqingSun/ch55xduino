#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Add model libs to existing bz2 package"
	echo
	echo "usage: $0 sdcc-snapshot-filename"
	exit 1
fi

VERBOSE=

FILE=$(realpath $1)

NAME=$(basename "$FILE")

# remove all suffixes
NAME=${NAME%%.t*}
NAME=${NAME%%.z*}

TMP=$(mktemp -d sdcc-repack-XXXXXX)
echo "Unpacking into $TMP..."

tar x${VERBOSE}jf "$FILE" -C "$TMP"


[ -d "$TMP/sdcc/share/sdcc/lib" ] && cp -r lib $TMP/sdcc/share/sdcc/
[ -d "$TMP/sdcc/lib" ] && cp -r lib $TMP/sdcc/

echo "Repacking into file $NAME"
#Stop OS X tar from including hidden ._ files in archives
export COPYFILE_DISABLE=1;
tar -jc${VERBOSE} --exclude="*DS_Store" -f "added/${NAME}_2.tar.bz2" -C "$TMP" sdcc

echo "cleaning up temporary files"
rm -rf "$TMP"

echo "done."