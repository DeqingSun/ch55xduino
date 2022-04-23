#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Minimize a SDCC snapshot build by leaving out all non-MCS51 files."
	echo
	echo "usage: $0 sdcc-snapshot-filename"
	exit 1
fi


#FILE=~/Downloads/sdcc-snapshot-amd64-unknown-linux2.5-20170919-9998.tar.bz2
FILE=$(realpath $1)

case "$FILE" in
	*.tar.bz2 | *.tbz)
		TARFLAG=j
		;;
	*.tar.gz | *.tgz)
		TARFLAG=z
		;;
	*.zip)
		TARFLAG=zip
		;;
esac

# set to v for verbose mode
VERBOSE=

# transform the filename from "*-snapshot-*" to "*-mcs51-*"
NAME=$(basename "$FILE")
NAME=${NAME/snapshot/mcs51}

# remove all suffixes
NAME=${NAME%%.t*}
NAME=${NAME%%.z*}

# patterns to exclude all unneeded files from unpacking
TAR_EXCLUDE='--exclude=doc	--exclude=src 	--exclude=non-free
--exclude=stlcs		--exclude=ds80c390.h	--exclude=pic*
--exclude=*00		--exclude=*08		--exclude=*2k
--exclude=include/mcs51		--exclude=*80		--exclude=*90
--exclude=*gb		--exclude=*ka		--exclude=*.info
--exclude=*pdk*		--exclude=*rab		--exclude=huge
--exclude=large*	--exclude=medium	--exclude=stm*
--exclude=mos6502*  --exclude=z80*      --exclude=sm83
--exclude=man --exclude=small-stack-auto
--exclude=bin/ucsim* --exclude=bin/as2gbmap --exclude=bin/makebin
--exclude=bin/packihx  --exclude=bin/s51 --exclude=bin/savr
--exclude=bin/*stm* --exclude=bin/sdcdb* --exclude=bin/sdnm
--exclude=bin/sdobjcopy --exclude=bin/sdranlib --exclude=bin/serialview
--exclude=bin/sm* --exclude=bin/sp* --exclude=bin/sr*  --exclude=bin/ss*  --exclude=bin/sx*
'

ZIP_EXCLUDE='-x */doc/* */man/* */non-free/* */src/* *stlcs* */pic*
*00* *08* *z80* *z180* *90* *2k* *gb* *ka* */info/* *pdk* *rab* *stm*
*/huge/* */large* */medium/* */mos6502* */sm83* */small-stack-auto/*
*/ucsim*.exe */include/mcs51/*
*/bin/makebin.exe */bin/packihx.exe */bin/s51.exe */bin/savr.exe
*/bin/sdcdb* */bin/sdnm.exe */bin/sdobjcopy.exe
*/bin/sdranlib.exe */bin/sm*.exe */bin/sp*.exe */bin/sr*.exe */bin/ss*.exe */bin/sx*.exe
'

TMP=$(mktemp -d sdcc-repack-XXXXXX)

echo "Unpacking into $TMP..."
if [ $TARFLAG == zip ]; then
#	unzip "$FILE" -d "$TMP" $ZIP_EXCLUDE
	cd "$TMP"
	unzip "$FILE" $ZIP_EXCLUDE
	cd -
else
	tar x${VERBOSE}${TARFLAG}f "$FILE" -C "$TMP" $TAR_EXCLUDE
fi

# always repack into a tar.bz2 file, even if was a zip before.
echo "Repacking into file $NAME"
#Stop OS X tar from including hidden ._ files in archives
export COPYFILE_DISABLE=1;
tar -jc${VERBOSE} --exclude="*DS_Store" -f "$NAME.tar.bz2" -C "$TMP" sdcc

echo "cleaning up temporary files"
rm -rf "$TMP"

echo "done."
